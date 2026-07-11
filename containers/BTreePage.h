//CBTreePage.h

/*************************
#ifndef BTPage_H
#define BTPage_H
***************************/
#ifndef CBTreePage_H
#define CBTreePage_H
#include <vector>
#include <cstddef>
#include <array>
#include <functional>
#include <mutex>
#include <string>
#include <sstream>
#include <iostream>
#include <assert.h>
#include "../types.h"
#include "general_iterator.h" 
using namespace std;
template <typename _keyType, typename _ObjIDType>
struct tagNode;
/**
 * @struct BTreeTraits
 * @brief Define los tipos asociados que utilizará el Árbol B.
 * @tparam _keyType Tipo de dato de la clave de indexación.
 * @tparam _ObjIDType Tipo de dato del identificador del objeto.
 */
template<typename _keyType, typename _ObjIDType>
struct BTreeTraits{
       using value_type     = _keyType;
       using ObjIDType      = _ObjIDType;
       using Node           = tagNode<value_type, ObjIDType>;
       //using Comp = less<value_type>;
};
/**
 * @struct Forward
 * @brief Política de iteración hacia adelante.
 */
struct Forward {
    static constexpr TF is_forward = true;
    static size_t extreme_child      (size_t keyCount) { return 0; }
    static size_t extreme_key        (size_t keyCount) { return 0; }
};
/**
 * @struct Backward
 * @brief Política de iteración hacia atrás.
 */
struct Backward {
    static constexpr TF is_forward = false; /**< Indica que la dirección es hacia atrás. */
    static size_t extreme_child      (size_t keyCount) { return keyCount; } /**< Devuelve el índice del hijo más a la derecha. */
    static size_t extreme_key        (size_t keyCount) { return keyCount - 1; } /**< Devuelve el índice de la última clave. */
};
template <typename Traits>
class BTree;
/**
 * @class BTreeIterator
 * @brief Iterador para recorrer el Árbol B.
 * @tparam Traits Tipos asociados del árbol.
 * @tparam Direction Política de dirección (Forward o Backward).
 */
template <typename Traits, typename Direction>
class BTreeIterator : public general_iterator<BTree<Traits>, BTreeIterator<Traits, Direction>> 
{
public:
    using Container = BTree<Traits>;
    using Base      = general_iterator<Container, BTreeIterator<Traits, Direction>>;
    using ptrPage   = typename Container::BTNode*;
    using IndexType = size_t;

private:
        /**
     * @struct PathNode
     * @brief Representa un nodo en el camino recorrido por el iterador.
     */

    struct PathNode {
        ptrPage page;/**< Puntero a la página del árbol. */
        IndexType index; /**< Índice de la clave actual en la página. */
    };
    static constexpr size_t MAX_HEIGHT = 16; /**< Altura máxima soportada para el iterador. */
    array<PathNode, MAX_HEIGHT> m_path; /**< Pila que almacena el camino desde la raíz. */
    size_t m_level;                     /**< Nivel actual en el camino. */
    /**
     * @brief Obtiene el nivel actual del iterador.
     * @return Nivel en el árbol.
     */
    size_t getLevel() {return m_level;}
    /**
     * @brief Actualiza el puntero base al nodo actual al que apunta el iterador.
     */
    void update_base_node();
    /**
     * @brief Avanza el iterador a la siguiente clave (orden ascendente).
     */
    void forward();
    /**
     * @brief Retrocede el iterador a la clave anterior (orden descendente).
     */
    void backward();

public:
/**
     * @brief Constructor por defecto.
     */
    BTreeIterator() : Base(nullptr, nullptr), m_level(0) {m_path[0] = {nullptr, 0};}
    
    /**
     * @brief Constructor que inicializa el iterador en una raíz específica.
     * @param pContainer Puntero al árbol contenedor.
     * @param root Puntero a la página raíz.
     */
    BTreeIterator(Container* pContainer, ptrPage root);
    
    /**
     * @brief Sobrecarga del operador de preincremento.
     * @return Referencia al iterador modificado.
     */
    BTreeIterator& operator++() 
    {
        if (!this->m_pNode || !m_path[m_level].page) return *this;
        if constexpr (Direction::is_forward) 
            forward();
        else 
            backward();
        update_base_node();
        return *this;
    }
};
template<typename Traits, typename Direction>
BTreeIterator<Traits,Direction>::BTreeIterator(Container* pContainer, ptrPage root)
: Base(pContainer, nullptr), m_level(0) {
        if (!root) {
            m_path[0] = {nullptr, 0};
            return;
        }
        ptrPage current = root;
        while (current) {
            m_path[m_level].page = current;
            if (current->m_SubPages[0] == nullptr) { 
                m_path[m_level].index = Direction::extreme_key(current->m_KeyCount);
                break;
            }
            size_t child_idx = Direction::extreme_child(current->m_KeyCount);
            m_path[m_level].index = Direction::extreme_key(current->m_KeyCount);
            current = current->m_SubPages[child_idx];
            m_level++;
        }
        update_base_node();
}
template<typename Traits, typename Direction>
void BTreeIterator<Traits,Direction>::update_base_node(){
    if (m_path[m_level].page != nullptr) 
        this->m_pNode = &(m_path[m_level].page->m_Keys[m_path[m_level].index]);
    else 
        this->m_pNode = nullptr;
}
template<typename Traits, typename Direction>
void BTreeIterator<Traits,Direction>::forward(){
        if (m_path[m_level].page->m_SubPages[m_path[m_level].index + 1]) {
            m_path[m_level].index++;
            m_level++;
            m_path[m_level].page = m_path[m_level - 1].page->m_SubPages[m_path[m_level - 1].index];
            m_path[m_level].index = 0; 
            while (m_path[m_level].page->m_SubPages[0]) {
                m_level++;
                m_path[m_level].page = m_path[m_level - 1].page->m_SubPages[0];
                m_path[m_level].index = 0;
            }
        } else {
            m_path[m_level].index++;
            while (m_level > 0 && m_path[m_level].index >= m_path[m_level].page->m_KeyCount) 
                m_level--; 
            if (m_level == 0 && m_path[m_level].index >= m_path[m_level].page->m_KeyCount) 
                m_path[m_level].page = nullptr;
        }
}
template<typename Traits, typename Direction>
void BTreeIterator<Traits,Direction>::backward(){
    if (m_path[m_level].page->m_SubPages[m_path[m_level].index]) {
            auto left_child = m_path[m_level].page->m_SubPages[m_path[m_level].index];
            m_path[m_level].index--;
            m_level++;
            m_path[m_level].page = left_child;
            m_path[m_level].index = m_path[m_level].page->m_KeyCount - 1; 
            while (m_path[m_level].page->m_SubPages[m_path[m_level].page->m_KeyCount]) {
                auto rightmost_child = m_path[m_level].page->m_SubPages[m_path[m_level].page->m_KeyCount];
                m_level++;
                m_path[m_level].page = rightmost_child;
                m_path[m_level].index = m_path[m_level].page->m_KeyCount - 1;
            }  
        } else {
            m_path[m_level].index--;
            while (m_level > 0 && m_path[m_level].index >= m_path[m_level].page->m_KeyCount)
                m_level--; 
            if (m_level == 0 && m_path[m_level].index >= m_path[m_level].page->m_KeyCount) 
                m_path[m_level].page = nullptr;
        }
}
// Si no lo encuentra, deberia decirme:
// cual es la posicion donde deberia estar

/**
 * @brief Búsqueda binaria genérica dentro de un contenedor.
 * @tparam Container Tipo del contenedor.
 * @tparam ObjType Tipo del objeto a buscar.
 * @param container Contenedor donde buscar.
 * @param first Índice inicial.
 * @param last Índice final.
 * @param object Objeto a buscar.
 * @return Posición donde se encuentra o debería insertarse el objeto.
 */
template <typename Container, typename ObjType>
size_t binary_search(Container& container, size_t first, size_t last, ObjType &object)
{
       if( first >= last )
               return first;
       while( first < last )
       {
               size_t mid = (first+last)/2;
               if( object == (ObjType)container[mid ] )
                       return mid;
               if( object > (ObjType)container[mid ] )
                       first = mid+1;
               else
                       last  = mid;
       }
       if( object <= (ObjType)container[first] )
               return first;
       return last;
}

/**
 * @brief Inserta un objeto en una posición específica de un contenedor, desplazando el resto.
 * @tparam Container Tipo del contenedor.
 * @tparam ObjType Tipo del objeto.
 * @param container Contenedor destino.
 * @param object Objeto a insertar.
 * @param pos Posición de inserción.
 */
template <typename Container, typename ObjType>
void insert_at(Container& container, const ObjType &object, size_t pos)
{
       size_t size = container.size();
       if (size>1)
                for(size_t i = size-1 ; i > pos ; i--)
                        container[i] = container[i-1];
       container[pos] =  object;
}

/**
 * @brief Elimina un elemento de un contenedor desplazando los elementos siguientes.
 * @tparam Container Tipo del contenedor.
 * @param container Contenedor origen.
 * @param pos Posición a eliminar.
 */
template <typename Container>
void remove(Container& container, size_t pos)
{
       size_t size = container.size();
       for(size_t i = pos+1 ; i < size ; i++)
               container[i-1] = container[i];
}

/**
 * @enum bt_ErrorCode
 * @brief Códigos de error y estado para las operaciones del Árbol B.
 */
enum bt_ErrorCode {
        bt_ok,          /**< Operación exitosa. */
        bt_overflow,    /**< Desbordamiento de página (necesita división). */
        bt_underflow,   /**< Subdesbordamiento de página (necesita fusión o redistribución). */
        bt_duplicate,   /**< Clave duplicada encontrada. */
        bt_nofound,     /**< Clave no encontrada. */
        bt_rootmerged   /**< La raíz fue fusionada y la altura del árbol disminuye. */
};

/*template <typename keyType>
bool operator>=(const _Node<keyType>& object1, const _Node<keyType>& object2)
{ return object1.key >= object2.key;    }

template <typename keyType>
bool operator<=(const _Node<keyType>& object1, const _Node<keyType>& object2)
{ return object1.key <= object2.key;    }*/

/**
 * @struct tagNode
 * @brief Representa un elemento individual (clave-valor) almacenado en una página.
 * @tparam keyType Tipo de la clave.
 * @tparam ObjIDType Tipo del identificador del objeto.
 */
template <typename keyType, typename ObjIDType>
struct tagNode
{
       keyType                 key;         /**< Clave de indexación. */
       ObjIDType               ObjID;       /**< Identificador del objeto asociado. */
       size_t                  UseCounter;  /**< Contador de uso del nodo. */
       tagNode(const keyType &_key, ObjIDType _ObjID)
               : key(_key), ObjID(_ObjID), UseCounter(0) {}
       tagNode(){}
       operator keyType()       { return key; }
       size_t   GetUseCounter() { return UseCounter;  }
       keyType   getKey()         const{return key;}
       ObjIDType getObjID()     const{return ObjID;}
       
};

/**
 * @class CBTreePage
 * @brief Representa una página (nodo) del Árbol B en memoria.
 * @tparam Traits Estructura de traits.
 */
template <typename Traits>
class CBTreePage 
// this is the in-memory version of the CBTreePage
{
        friend class BTree<Traits>;
        template <typename T, typename D> friend class BTreeIterator;
        using keyType   = typename Traits::value_type;
        using ObjIDType = typename Traits::ObjIDType;
        using Node      = typename Traits::Node;
        using BTPage    =  CBTreePage<Traits>;// useful shorthand
 public:
       /**
        * @brief Constructor de la página.
        * @param maxKeys Cantidad máxima de claves.
        * @param unique Indica si no permite duplicados.
        */
       CBTreePage(size_t maxKeys, TF unique = true);
       virtual ~CBTreePage();

       bt_ErrorCode    Insert (const keyType &key, const ObjIDType ObjID);
       bt_ErrorCode    Remove (const keyType &key, const ObjIDType ObjID);
       TF              Search (const keyType &key, ObjIDType &ObjID);
       void            Print  (ostream &os);
protected:
       size_t  m_MinKeys;          /**< Número mínimo de claves en el nodo. */
       size_t  m_MaxKeys;          /**< Número máximo de claves en el nodo. */
       size_t  m_MaxKeysForChilds; /**< Máximo de claves para páginas hijas (distingue raíz). */
       TF m_Unique;                /**< Flag para restringir duplicados. */
       TF m_isRoot;                /**< Flag para identificar si es nodo raíz. */
       vector<Node>       m_Keys;      /**< Array de claves del nodo. */
       vector<BTPage *>   m_SubPages;  /**< Array de punteros a hijos. */
       size_t  m_KeyCount;             /**< Cantidad actual de claves almacenadas. */

       /** @brief Inicializa los vectores de la página. */
       void  Create();

       /** @brief Libera la memoria de las subpáginas. */
       void  Reset ();

       /** @brief Resetea y destruye la instancia actual. */
       void  Destroy () {   Reset(); delete this;}

       /** @brief Limpia el contador de claves sin borrar memoria. */
       void  clear ();

       /**
        * @brief Intenta redistribuir claves con un hermano adyacente.
        * @param pos Índice del hijo que sufrió el underflow o overflow.
        * @return true si la redistribución fue exitosa.
        */
       TF  Redistribute1   (size_t &pos);
       /**
        * @brief Intenta redistribuir claves considerando los dos hermanos adyacentes.
        * @param pos Índice del hijo conflictivo.
        * @return true si la redistribución cruzada fue exitosa.
        */
       TF  Redistribute2   (size_t pos);

       /**
        * @brief Mueve claves de la página hermana derecha a la izquierda (Right to Left).
        * @param pos Índice de la página de origen en m_SubPages.
        */
       void  RedistributeR2L (size_t pos);

       /**
        * @brief Mueve claves de la página hermana izquierda a la derecha (Left to Right).
        * @param pos Índice de la página de origen en m_SubPages.
        */
       void  RedistributeL2R (size_t pos);

       /**
        * @brief Gestiona una condición de subdesbordamiento intentando redistribuciones.
        * @param pos Índice del hijo con underflow.
        * @return true si se resolvió el underflow.
        */
       TF    TreatUnderflow  (size_t &pos)
       {       return Redistribute1(pos) || Redistribute2(pos);}
       
       /**
        * @brief Fusiona (merge) un nodo hijo con uno de sus hermanos.
        * @param pos Índice del hijo.
        * @return Código de error (bt_ok o bt_underflow si esto causó underflow en el padre).
        */
       bt_ErrorCode    Merge  (size_t pos);

       /**
        * @brief Fusiona los hijos de la raíz cuando su capacidad se reduce al mínimo.
        * @return bt_rootmerged si la raíz fue colapsada.
        */
       bt_ErrorCode    MergeRoot ();

       /**
        * @brief Divide (split) una página hija que ha excedido su límite de claves (overflow).
        * @param pos Índice del hijo que será dividido.
        */
       void  SplitChild (size_t pos);
       
       /**
        * @brief Obtiene el primer nodo (más a la izquierda) en este subárbol.
        * @return Referencia al nodo más pequeño.
        */
       Node &GetFirstNode();

       // --- Funciones auxiliares de estado ---
       TF Overflow()  { return m_KeyCount > m_MaxKeys; } /**< Evalúa si hay desbordamiento. */
       TF Underflow() { return m_KeyCount < MinNumberOfKeys(); } /**< Evalúa si hay subdesbordamiento. */
       TF IsFull()    { return m_KeyCount >= m_MaxKeys; } /**< Evalúa si la página está llena. */
       size_t  MinNumberOfKeys()  { return 2*m_MaxKeys/3.0; } /**< Calcula el número mínimo permitido de claves. */
       size_t  GetFreeCells()  { return m_MaxKeys - m_KeyCount; } /**< Devuelve la cantidad de espacios libres. */
       size_t& NumberOfKeys()  { return m_KeyCount; } /**< Retorna referencia a la cantidad de claves. */
       size_t  GetNumberOfKeys()  { return m_KeyCount; } /**< Retorna el número de claves. */
       TF IsRoot()  { return m_MaxKeysForChilds != m_MaxKeys; } /**< Evalúa si es el nodo raíz (por configuración de capacidad). */
       
       /**
        * @brief Ajusta el número máximo de claves que tendrán las páginas descendientes.
        * @param orderforchilds Nuevo límite máximo.
        */
       void SetMaxKeysForChilds(size_t orderforchilds)
       {
               m_MaxKeysForChilds = orderforchilds;
       }

       size_t GetFreeCellsOnLeft(size_t pos);  /**< Obtiene espacios libres en el hermano izquierdo. */
       size_t GetFreeCellsOnRight(size_t pos); /**< Obtiene espacios libres en el hermano derecho. */
       template <typename Func, typename... Args>
       void ForEach(size_t level, Func func, Args &&... args);
       template <typename Func, typename... Args>
       Node* FirstThat( size_t level, Func func, Args &&... args);

private:
       /**
        * @brief Divide el nodo raíz en tres nodos cuando se desborda.
        * @return true si la división fue exitosa.
        */
       TF SplitRoot();

       /**
        * @brief Función de soporte para dividir el contenido de páginas excedidas en tres páginas distintas.
        * @param tmpKeys Vector temporal consolidado de claves.
        * @param SubPages Vector temporal consolidado de punteros a hijos.
        * @param[out] pChild1 Primera nueva página.
        * @param[out] pChild2 Segunda nueva página.
        * @param[out] pChild3 Tercera nueva página.
        * @param[out] oi1 Clave que asciende al padre (promoción 1).
        * @param[out] oi2 Clave que asciende al padre (promoción 2).
        */
       void SplitPageInto3(vector<Node>   & tmpKeys,
                                               vector<BTPage *>  & SubPages,
                                               BTPage           *& pChild1,
                                               BTPage           *& pChild2,
                                               BTPage           *& pChild3,
                                               Node        & oi1,
                                               Node        & oi2);
        
       /**
        * @brief Mueve todo el contenido de una página hija hacia los vectores temporales (usado en merge y splits).
        * @param pChildPage Página de origen.
        * @param[out] tmpKeys Vector destino para claves.
        * @param[out] tmpSubPages Vector destino para punteros.
        */
       void MovePage(BTPage *  pChildPage,vector<Node> & tmpKeys,vector<BTPage *> & tmpSubPages);
};

template <typename Traits>
CBTreePage<Traits>::CBTreePage(size_t maxKeys, TF unique)
                                       : m_MaxKeys(maxKeys), m_Unique(unique), m_KeyCount(0)
{
       Create();
       SetMaxKeysForChilds(m_MaxKeys);
}

template <typename Traits>
CBTreePage<Traits>::~CBTreePage()
{
       Reset();
}

template <typename Traits>
bt_ErrorCode CBTreePage<Traits>::Insert(const keyType& key, const ObjIDType ObjID)
{
       size_t pos = binary_search(m_Keys, 0, m_KeyCount, key);
       bt_ErrorCode error = bt_ok;

       if( pos < m_KeyCount && (keyType)m_Keys[pos] == key && m_Unique)
               return bt_duplicate; // this key is duplicate

       if( !m_SubPages[pos] ) // this is a leave
       {
               ::insert_at(m_Keys, Node(key, ObjID), pos);
               NumberOfKeys()++;
               if( Overflow() )
                       return bt_overflow;
               return bt_ok;
       }
        // recursive insertion
        error = m_SubPages[pos]->Insert(key, ObjID);
        if( error == bt_overflow )
        {
                if( !Redistribute1(pos) )
                        SplitChild(pos);
                if( Overflow() )          // Propagate overflow
                        return bt_overflow;
                return bt_ok;
        }
       return bt_ok;
}

template <typename Traits>
TF CBTreePage<Traits>::Redistribute1(size_t &pos)
{
       if( m_SubPages[pos]->Underflow() )
       {       // nkol = Number of keys on left brother, nkor = Number of keys on right brother
               size_t nkol = 0,
                      nkor = 0;
               // is this the first element or there are more elements on right brother
               if( pos > 0 )
                       nkol = m_SubPages[pos-1]->NumberOfKeys();
               if( pos < NumberOfKeys() )
                       nkor = m_SubPages[pos+1]->NumberOfKeys();

               if( nkol > nkor ){
                       if( m_SubPages[pos-1]->NumberOfKeys() > m_SubPages[pos-1]->MinNumberOfKeys() )
                               RedistributeL2R(pos-1); // bring elements from left brother
                       else{
                               if( pos == NumberOfKeys() )
                                        --pos;
                                return false;
                       }
               }else{//nkol < nkor )
                       if( m_SubPages[pos+1]->NumberOfKeys() > m_SubPages[pos+1]->MinNumberOfKeys() )
                               RedistributeR2L(pos+1); // bring elements from right brother
                       else{
                               if( pos == 0 )
                                       ++pos;
                               return false;
                       }
               }
       }
       else // it is due to overflow
       {
               size_t fcol = GetFreeCellsOnLeft(pos),   // Free Cells On Left
                   fcor = GetFreeCellsOnRight(pos);  // Free Cells On Right

               if( !fcol && !fcor && m_SubPages[pos]->IsFull() )
                       return false;
               if( fcol > fcor ) // There is more space on left
                       RedistributeR2L(pos);
               else
                       RedistributeL2R(pos);

       }
       return true;
}

// Redistribute2 function
// it considers two brothers m_SubPages[pos-1] && m_SubPages[pos+1]
// if it fails the only way is merge !
template <typename Traits>
TF CBTreePage<Traits>::Redistribute2(size_t pos)
{
       assert( pos > 0 && pos < NumberOfKeys()  );
       assert( m_SubPages[pos-1] != 0 && m_SubPages[pos] != 0 && m_SubPages[pos+1] != 0 );
       assert( m_SubPages[pos-1]->Underflow() ||
                       m_SubPages[ pos ]->Underflow() ||
                       m_SubPages[pos+1]->Underflow() );

       if( m_SubPages[pos-1]->Underflow() )
       {       // Rotate R2L
               RedistributeR2L(pos+1);
               RedistributeR2L(pos);
               if( m_SubPages[pos-1]->Underflow() )
                       return false;
       }
       else if( m_SubPages[pos+1]->Underflow() )
       {       // Rotate L2R
               RedistributeL2R(pos-1);
               RedistributeL2R(pos);
               if( m_SubPages[pos+1]->Underflow() )
                       return false;
       }
       else // The problem is exactly at pos !
       {
               // Rotate L2R
               RedistributeL2R(pos-1);
               RedistributeR2L(pos+1);
               if( m_SubPages[pos]->Underflow() )
                       return false;
       }
       return true;
}

template <typename Traits>
void CBTreePage<Traits>::RedistributeR2L(size_t pos)  
{
       BTPage  *pSource = m_SubPages[ pos ],
                       *pTarget = m_SubPages[pos-1];

       while(pSource->GetNumberOfKeys() > pSource->MinNumberOfKeys() &&
             pTarget->GetNumberOfKeys() < pSource->GetNumberOfKeys() )
       {
               // Move from this page to the down-left page \/
               ::insert_at(pTarget->m_Keys, m_Keys[pos-1], pTarget->NumberOfKeys()++);
               // Move the pointer leftest pointer to the rightest position
               ::insert_at(pTarget->m_SubPages, pSource->m_SubPages[0], pTarget->NumberOfKeys());

               // Move the leftest element to the root
               m_Keys[pos-1] = pSource->m_Keys[0];

               // Remove the leftest element from rigth page
               ::remove(pSource->m_Keys    , 0);
               ::remove(pSource->m_SubPages, 0);
               pSource->NumberOfKeys()--;
       }
}

template <typename Traits>
void CBTreePage<Traits>::RedistributeL2R(size_t pos)
{
       BTPage  *pSource = m_SubPages[pos],
                       *pTarget = m_SubPages[pos+1];
       while(pSource->GetNumberOfKeys() > pSource->MinNumberOfKeys() &&
                 pTarget->GetNumberOfKeys() < pSource->GetNumberOfKeys() )
       {
               // Move from this page to the down-RIGHT page \/
               ::insert_at(pTarget->m_Keys, m_Keys[pos], 0);
               // Move the pointer rightest pointer to the leftest position
               ::insert_at(pTarget->m_SubPages, pSource->m_SubPages[pSource->NumberOfKeys()], 0);
               pTarget->NumberOfKeys()++;

               // Move the rightest element to the root
               m_Keys[pos] = pSource->m_Keys[pSource->NumberOfKeys()-1];

               // Remove the leftest element from rigth page
               // it is not necessary erase because m_KeyCount controls
               pSource->NumberOfKeys()--;
       }
}

template <typename Traits>
void CBTreePage<Traits>::SplitChild(size_t pos)
{
       // FIRST: deciding the second page to split
       BTPage  *pChild1 = 0, *pChild2 = 0;
       if( pos > 0 )                                   // is left page full ?
               if( m_SubPages[pos-1]->IsFull() )
               {
                       pChild1 = m_SubPages[pos-1];
                       pChild2 = m_SubPages[pos--];
               }
       if( pos < GetNumberOfKeys() )   // is right page full ?
               if( m_SubPages[pos+1]->IsFull() )
               {
                       pChild1 = m_SubPages[pos];
                       pChild2 = m_SubPages[pos+1];
               }

       size_t nKeys = pChild1->GetNumberOfKeys() + pChild2->GetNumberOfKeys() + 1;

       // SECOND: copy both pages to a temporal one
       // Create two tmp vector
       vector<Node> tmpKeys;
       //tmpKeys.resize(nKeys);
       vector<BTPage *>   tmpSubPages;
       //tmpKeys.resize(nKeys+1);

       // Prepara el vectpor unificado de las 2 paginas a ser divididas en 3
       // copy from left child
       MovePage(pChild1, tmpKeys, tmpSubPages);
       // copy a key from parent
       tmpKeys    .push_back(m_Keys[pos]);

       // copy from right child
       MovePage(pChild2, tmpKeys, tmpSubPages);

       BTPage *pChild3 = 0;
       Node oi1, oi2;
       SplitPageInto3(tmpKeys, tmpSubPages, pChild1, pChild2, pChild3, oi1, oi2);

       // copy the first element to the root
       m_Keys    [pos] = oi1;
       m_SubPages[pos] = pChild1;

       // copy the second element to the root
       ::insert_at(m_Keys, oi2, pos+1);
       ::insert_at(m_SubPages, pChild2, pos+1);
       NumberOfKeys()++;

       m_SubPages[pos+2] = pChild3;
}

template <typename Traits>
void CBTreePage<Traits>::SplitPageInto3(vector<Node>& tmpKeys,
                                                vector<BTPage *>  & tmpSubPages,
                                                BTPage*                   &     pChild1,
                                                BTPage*                   &     pChild2,
                                                BTPage*                   &     pChild3,
                                                Node                & oi1,
                                                Node                & oi2)
{
       assert(tmpKeys.size() >= 8);
       assert(tmpSubPages.size() >= 9);
       if( !pChild1 )
               pChild1 = new BTPage(m_MaxKeysForChilds, m_Unique);

       // Split tmpKeys page into 3 pages
       // copy 1/3 elements to the first child
       pChild1->clear();
       size_t nKeys = (tmpKeys.size()-2)/3;
       size_t i = 0;
       for( ; i < nKeys; i++ )
       {
               pChild1->m_Keys    [i] = tmpKeys    [i];
               pChild1->m_SubPages[i] = tmpSubPages[i];
               pChild1->NumberOfKeys()++;
       }
       pChild1->m_SubPages[i] = tmpSubPages[i];

       // first element to go up !
       oi1 = tmpKeys[i++];

       if( !pChild2 )
               pChild2 = new BTPage(m_MaxKeysForChilds, m_Unique);
       pChild2->clear();
       // copy 1/3 to the second child
       nKeys += (tmpKeys.size()-2)/3 + 1;
       size_t j = 0;
       for(; i < nKeys; i++, j++ )
       {
               pChild2->m_Keys    [j] = tmpKeys    [i];
               pChild2->m_SubPages[j] = tmpSubPages[i];
               pChild2->NumberOfKeys()++;
       }
       pChild2->m_SubPages[j] = tmpSubPages[i];

       // copy the second element to the root
       oi2 = tmpKeys[i++];

       // copy 1/3 to the third child
       if( !pChild3 )
               pChild3 = new BTPage(m_MaxKeysForChilds, m_Unique);
       pChild3->clear();
       nKeys = tmpKeys.size();
       for(j = 0; i < nKeys; i++, j++)
       {
               pChild3->m_Keys    [j] = tmpKeys    [i];
               pChild3->m_SubPages[j] = tmpSubPages[i];
               pChild3->NumberOfKeys()++;
       }
       pChild3->m_SubPages[j] = tmpSubPages[i];
}

template <typename Traits>
TF CBTreePage<Traits>::SplitRoot()
{
       BTPage  *pChild1 = 0, *pChild2 = 0, *pChild3 = 0;
       Node oi1, oi2;
       SplitPageInto3( m_Keys,m_SubPages,pChild1, pChild2, pChild3, oi1, oi2);
       clear();

       // copy the first element to the root
       m_Keys    [0] = oi1;
       m_SubPages[0] = pChild1;
       NumberOfKeys()++;

       // copy the second element to the root
       m_Keys    [1] = oi2;
       m_SubPages[1] = pChild2;
       NumberOfKeys()++;

       m_SubPages[2] = pChild3;
       return true;
}

template <typename Traits>
TF CBTreePage<Traits>::Search(const keyType &key, ObjIDType &ObjID)
{
       size_t pos = binary_search(m_Keys, 0, m_KeyCount, key);
       if( pos >= m_KeyCount ){
               if( m_SubPages[pos] )
                       return m_SubPages[pos]->Search(key, ObjID);
               else
                       return false;
       }
       if( key == m_Keys[pos].key )
       {
               ObjID = m_Keys[pos].ObjID;
               m_Keys[pos].UseCounter++;
               return true;
       }
       if( key < m_Keys[pos].key )
               if( m_SubPages[pos] )
                       return m_SubPages[pos]->Search(key, ObjID);
       return false;
}


template <typename Traits>
template <typename Func, typename... Args>
void CBTreePage<Traits>::ForEach( size_t level, Func func, Args &&... args)
{
       for( size_t i = 0 ; i < m_KeyCount ; i++)
       {
               if( m_SubPages[i] )
                       m_SubPages[i]->ForEach( level+1, func, forward<Args>(args)...);
               func(m_Keys[i], level, args...);
       }
       if( m_SubPages[m_KeyCount] )
               m_SubPages[m_KeyCount]->ForEach( level+1, func, forward<Args>(args)...);
}

template <typename Traits>
template <typename Func, typename... Args>
typename CBTreePage<Traits>::Node *
CBTreePage<Traits>::FirstThat(size_t level, Func func, Args &&... args)
{
       Node *pTmp;
       for( size_t i = 0 ; i < m_KeyCount ; i++)
       {
               if( m_SubPages[i] ){
                        pTmp = m_SubPages[i]->FirstThat( level+1, forward<Args>(args)...);
                       if( pTmp )
                               return pTmp;
               }
               if( func(m_Keys[i], level, args...) )
                       return &m_Keys[i];
       }
       if( m_SubPages[m_KeyCount] ){
                pTmp = m_SubPages[m_KeyCount]->FirstThat( level+1, forward<Args>(args)...);
               if( pTmp )
                       return pTmp;
       }
       return 0;
}

template <typename Traits>
bt_ErrorCode CBTreePage<Traits>::Remove(const keyType &key, const ObjIDType ObjID)
{
       bt_ErrorCode error = bt_ok;
       size_t pos = binary_search(m_Keys, 0, m_KeyCount, key);
       if( pos < NumberOfKeys() && key == m_Keys[pos].key /*&& m_Keys[pos].m_ObjID == ObjID*/) // We found it !
       {
               // This is a leave: First
               if( !m_SubPages[pos+1] )  // This is a leave ? FIRST CASE !
               {
                       ::remove(m_Keys, pos);
                       NumberOfKeys()--;
                       if( Underflow() )
                               return bt_underflow;
                       return bt_ok;
               }

               // We FOUND IT BUT it is NOT a leave ? SECOND CASE !
               {
                       // Get the first element from right branch
                       Node &rFirstFromRight = m_SubPages[pos+1]->GetFirstNode();
                       // change with a leave
                       swap(m_Keys[pos], rFirstFromRight);
                       // Remove it from this leave

                       //Print(cout);
                       error = m_SubPages[++pos]->Remove(key, ObjID);
               }
       }
       else if( pos == NumberOfKeys() ) // it is not here, go by the last branch
               error = m_SubPages[pos]->Remove(key, ObjID);
       else if( key <= m_Keys[pos].key ){ // = is because identical keys are inserted on left (see Insert)
               if( m_SubPages[pos] )
                       error = m_SubPages[pos]->Remove(key, ObjID);
               else
                       return bt_nofound;
       }
       if( error == bt_underflow ){
               // THIRD CASE: After removing the element we have an underflow
               //Print(cout);
               if( TreatUnderflow(pos) )
                       return bt_ok;
               // FOURTH CASE: it was not possible to redistribute -> Merge
               if( IsRoot() && NumberOfKeys() == 2 )
                       return MergeRoot();
               return Merge(pos);
       }
       if( error == bt_nofound )
               return bt_nofound;
       return bt_ok;
}


template <typename Traits>
bt_ErrorCode CBTreePage<Traits>::Merge(size_t pos)
{
       assert( m_SubPages[pos-1]->NumberOfKeys() +
                m_SubPages[ pos ]->NumberOfKeys() +
                m_SubPages[pos+1]->NumberOfKeys() ==
                3*m_SubPages[ pos ]->MinNumberOfKeys() - 1);

       // FIRST: Put all the elements into a vector
       vector<Node> tmpKeys;
       //tmpKeys.resize(nKeys);
       vector<BTPage *>   tmpSubPages;

       BTPage  *pChild1 = m_SubPages[pos-1],
                       *pChild2 = m_SubPages[ pos ],
                       *pChild3 = m_SubPages[pos+1];
       MovePage(pChild1, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[pos-1]);
       MovePage(pChild2, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[ pos ]);
       MovePage(pChild3, tmpKeys, tmpSubPages);
       pChild3->Destroy();;

       // Move 1/2 elements to pChild1
       size_t nKeys = pChild1->GetFreeCells();
       size_t i = 0;
       for( ; i < nKeys ; i++ )
       {
               pChild1->m_Keys    [i] = tmpKeys    [i];
               pChild1->m_SubPages[i] = tmpSubPages[i];
               pChild1->NumberOfKeys()++;
       }
       pChild1->m_SubPages[i] = tmpSubPages[i];

       m_Keys    [pos-1] = tmpKeys[i];
       m_SubPages[pos-1] = pChild1;

       ::remove(m_Keys    , pos);
       ::remove(m_SubPages, pos);
       NumberOfKeys()--;

       nKeys = pChild2->GetFreeCells();
       size_t j = ++i;
       for(i = 0 ; i < nKeys ; i++, j++ )
       {
               pChild2->m_Keys    [i] = tmpKeys    [j];
               pChild2->m_SubPages[i] = tmpSubPages[j];
               pChild2->NumberOfKeys()++;
       }
       pChild2->m_SubPages[i] = tmpSubPages[j];
       m_SubPages[ pos ]          = pChild2;

       if( Underflow() )
               return bt_underflow;
       return bt_ok;
}

template <typename Traits>
bt_ErrorCode CBTreePage<Traits>::MergeRoot()
{
       size_t pos = 1;
       assert( m_SubPages[pos-1]->NumberOfKeys() +
                       m_SubPages[ pos ]->NumberOfKeys() +
                       m_SubPages[pos+1]->NumberOfKeys() ==
                       3*m_SubPages[ pos ]->MinNumberOfKeys() - 1);

       BTPage  *pChild1 = m_SubPages[pos-1], *pChild2 = m_SubPages[ pos ], *pChild3 = m_SubPages[pos+1];
       size_t nKeys = pChild1->NumberOfKeys() + pChild2->NumberOfKeys() + pChild3->NumberOfKeys() + 2;

       // FIRST: Put all the elements into a vector
       vector<Node> tmpKeys;
       //tmpKeys.resize(nKeys);
       vector<BTPage *>   tmpSubPages;

       MovePage(pChild1, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[pos-1]);
       MovePage(pChild2, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[ pos ]);
       MovePage(pChild3, tmpKeys, tmpSubPages);

       clear();
       size_t i = 0;
       for( ; i < nKeys ; i++ ){
               m_Keys    [i] = tmpKeys    [i];
               m_SubPages[i] = tmpSubPages[i];
               NumberOfKeys()++;
       }
       m_SubPages[i] = tmpSubPages[i];

       //Print(cout);
       pChild1->Destroy();
       pChild2->Destroy();
       pChild3->Destroy();

       return bt_rootmerged;
}

template <typename Traits>
typename CBTreePage<Traits>::Node &
CBTreePage<Traits>::GetFirstNode()
{
       if( m_SubPages[0] )
               return m_SubPages[0]->GetFirstNode();
       return m_Keys[0];
}


template <typename Traits>
void CBTreePage<Traits>::Print(ostream & os)
{

        ForEach( 0,[&os](const Node& node, size_t level){
                for(size_t i=0;i<level; i++)
                        os<<"\t";
                os<<node.getKey()<<"->"<<node.getObjID()<<"\n";
        });
}

template <typename Traits>
void CBTreePage<Traits>::Create()
{
       Reset();
       m_Keys.resize(m_MaxKeys+1);
       m_SubPages.resize(m_MaxKeys+2, NULL);
       m_KeyCount = 0;
       m_MinKeys  = 2 * m_MaxKeys/3;
}

template <typename Traits>
void CBTreePage<Traits>::Reset()
{
       for( size_t i = 0 ; i < m_KeyCount ; i++ )
               delete m_SubPages[i];
       clear();
}

template <typename Traits>
void CBTreePage<Traits>::clear()
{
       //m_Keys.clear();
       //m_SubPages.clear();
       m_KeyCount = 0;
}

template <typename Traits>
CBTreePage<Traits> * CreateBTreeNode (size_t maxKeys, TF unique)
{
       return new CBTreePage<Traits> (maxKeys, unique);
}

template <typename Traits>
void CBTreePage<Traits>::MovePage(BTPage *pChildPage, vector<Node> &tmpKeys,vector<BTPage *> &tmpSubPages)
{
       size_t nKeys = pChildPage->GetNumberOfKeys();
       size_t i = 0;
       for( ; i < nKeys; i++ )
       {
               tmpKeys    .push_back(pChildPage->m_Keys[i]);
               tmpSubPages.push_back(pChildPage->m_SubPages[i]);
       }
       tmpSubPages.push_back(pChildPage->m_SubPages[i]);
       pChildPage->clear();
}

template <typename Traits>
size_t CBTreePage<Traits>::GetFreeCellsOnLeft(size_t pos)
{
       if( pos > 0 )                                   // there is some page on left ?
               return m_SubPages[pos-1]->GetFreeCells();
       return 0;
}

template <typename Traits>
size_t CBTreePage<Traits>::GetFreeCellsOnRight(size_t pos)
{
       if( pos < GetNumberOfKeys() )   // there is some page on right ?
               return m_SubPages[pos+1]->GetFreeCells();
       return 0;
}

#endif