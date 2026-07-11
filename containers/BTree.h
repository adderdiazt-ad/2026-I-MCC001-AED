// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include "BTreePage.h"
#include "../types.h" 
#include "../foreach.h"
     

#define DEFAULT_BTREE_ORDER 3
using namespace std;

/**
 * @class BTree
 * @brief Estructura principal del Árbol B (B-Tree).
 * Contenedor genérico thread-safe para operaciones de búsqueda, inserción y eliminación.
 * @tparam Traits Estructura con tipos (value_type, ObjIDType, Node).
 */
template <typename Traits>
class BTree {
public:
        using BTNode            =   CBTreePage<Traits>;// useful shorthand
        using keyType           =   typename Traits::value_type;
        using ObjIDType         =   typename Traits::ObjIDType;
        using Node              =   typename BTNode::Node;

        using forwardIterator   =   BTreeIterator<Traits,Forward>;
        using backwardIterator  =   BTreeIterator<Traits,Backward>;

public:
       /**
        * @brief Constructor del árbol.
        * @param order Orden del árbol (determina capacidad máxima = 2*order).
        * @param unique Limita a valores únicos.
        */
       BTree(size_t order = DEFAULT_BTREE_ORDER, TF unique = true);
       /**
        * @brief Destructor del árbol.
        */
       ~BTree();

       /**
        * @brief Inserta un elemento en el árbol. Protegido por un mutex recursivo.
        * @param key Clave de indexación.
        * @param ObjID Identificador del objeto a almacenar.
        * @return true si la inserción fue exitosa, false si la clave es un duplicado no permitido.
        */
       TF            Insert (const keyType key, const ObjIDType ObjID);
       
       /**
        * @brief Elimina un elemento del árbol. Protegido por un mutex recursivo.
        * @param key Clave de indexación.
        * @param ObjID Identificador del objeto a eliminar.
        * @return true si la eliminación fue exitosa, false si la clave no se encuentra.
        */
       TF            Remove (const keyType key, const ObjIDType ObjID);
       
       /**
        * @brief Realiza la búsqueda de una clave. Protegido por mutex.
        * @param key Clave buscada.
        * @return Identificador del objeto si se encuentra, o -1 en caso de error.
        */
       auto          Search (const keyType key);
       
       /** @brief Retorna el número de elementos contenidos. Thread-safe. */
       size_t        size()         {scoped_lock<recursive_mutex> lock(m_tree_mutex); return m_NumKeys; }
       
       /** @brief Retorna la altura (profundidad) del árbol. Thread-safe. */
       size_t        height()       {scoped_lock<recursive_mutex> lock(m_tree_mutex); return m_Height;  }
       
       /** @brief Devuelve el orden con el que se inicializó el árbol. */
       size_t        GetOrder()     { return m_Order;   }
       
       /**
        * @brief Serializa el contenido del árbol a una cadena de texto.
        * @return Representación en formato string (clave->valor).
        */
       string        to_string();
       
       /**
        * @brief Imprime la topología del árbol en el flujo dado mostrando niveles jerárquicos.
        * @param os Flujo de salida estándar u archivo.
        */
       void          printGraphic (ostream &os);

       auto         beginForward()  {return forwardIterator(this,&m_Root);}
       auto         endForward()    {return forwardIterator();}
       auto         beginBackward() {return backwardIterator(this,&m_Root);}
       auto         endBackward()   {return backwardIterator();}
    
    /**
     * @struct rangeView
     * @brief Estructura de utilidad para gestionar la iteración bloqueando el árbol para concurrencia.
     * @tparam Iterator Tipo de iterador usado (Forward/Backward).
     */
    template <typename Iterator>
    struct rangeView {
        Iterator m_begin;
        Iterator m_end;
        unique_lock<recursive_mutex> m_lock;
        rangeView(Iterator begin, Iterator end, recursive_mutex& mutex) 
            : m_begin(begin), m_end(end), m_lock(mutex) {}
        Iterator begin(){ return m_begin; }
        Iterator end()  { return m_end; }

        template <typename Func, typename... Args>
        decltype(auto) walk(Func func, Args &&...  args){
            return ::walk(m_begin, m_end, func, std::forward<Args>(args)... );
        }
    };

    /** @brief Genera una vista de rango para recorrido de inicio a fin. */
    auto forward() {return rangeView<forwardIterator>(beginForward(),endForward(),m_tree_mutex);}
    
    /** @brief Genera una vista de rango para recorrido de fin a inicio. */
    auto backward(){return rangeView<backwardIterator>(beginBackward(),endBackward(),m_tree_mutex);}

protected:
       BTNode          m_Root;        /**< Instancia del nodo raíz (CBTreePage). */
       size_t          m_Height;      /**< Altura actual del árbol B. */
       size_t          m_Order;       /**< Orden configurado. */
       size_t          m_NumKeys;     /**< Contador total de las claves almacenadas. */
       TF              m_Unique;      /**< Bandera que indica si el árbol acepta datos repetidos. */
       recursive_mutex m_tree_mutex;  /**< Mutex que serializa los accesos y modificaciones en el árbol. */
};

const size_t MaxHeight = 5;
template <typename Traits>
BTree<Traits>::BTree(size_t order, TF unique)
                               : m_Unique(unique),
                                 m_Order(order),
                                 m_Root(2 * order  + 1, unique),
                                 m_NumKeys(0)
{
       m_Root.SetMaxKeysForChilds(order);
       m_Height = 1;
}

template <typename Traits>
BTree<Traits>::~BTree()
{
}

template <typename Traits>
TF BTree<Traits>::Insert(const keyType key, const ObjIDType ObjID)
{      scoped_lock<recursive_mutex> lock(m_tree_mutex);
       bt_ErrorCode error = m_Root.Insert(key, ObjID);
       if( error == bt_duplicate )
               return false;
       m_NumKeys++;
       if( error == bt_overflow )
       {
               m_Root.SplitRoot();
               m_Height++;
       }
       return true;
}

template <typename Traits>
TF BTree<Traits>::Remove (const keyType key, const ObjIDType ObjID)
{      scoped_lock<recursive_mutex> lock(m_tree_mutex);
       bt_ErrorCode error = m_Root.Remove(key, ObjID);
       if( error == bt_duplicate || error == bt_nofound )
               return false;
       m_NumKeys--;

       if( error == bt_rootmerged )
               m_Height--;
       return true;
}

template <typename Traits>
auto BTree<Traits>::Search (const keyType key)
{      scoped_lock<recursive_mutex> lock(m_tree_mutex);
       ObjIDType ObjID = -1;
       m_Root.Search(key, ObjID);
       return ObjID;
}
template <typename Traits>
string BTree<Traits>::to_string(){
    std::ostringstream ss;
    this->forward().walk([&ss](const auto& node){
        ss<<node.getKey()<<"->"<<node.getObjID()<<"\n";
    
    });
    return ss.str();
}
template <typename Traits>
void BTree<Traits>::printGraphic(ostream &os){
       scoped_lock<recursive_mutex> lock(m_tree_mutex);
       m_Root.Print(os);
}

#endif