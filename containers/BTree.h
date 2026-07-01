// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include "BTreePage.h"
#include "../types.h" 
#include "../foreach.h"
     

#define DEFAULT_BTREE_ORDER 3
using namespace std;

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
       BTree(size_t order = DEFAULT_BTREE_ORDER, TF unique = true);
       ~BTree();
       TF            Insert (const keyType key, const ObjIDType ObjID);
       TF            Remove (const keyType key, const ObjIDType ObjID);
       auto          Search (const keyType key);
       size_t        size()         {scoped_lock<recursive_mutex> lock(m_tree_mutex); return m_NumKeys; }
       size_t        height()       {scoped_lock<recursive_mutex> lock(m_tree_mutex); return m_Height;  }
       size_t        GetOrder()     { return m_Order;   }
       string        to_string(); 
       void          printGraphic (ostream &os);

       auto         beginForward()  {return forwardIterator(this,&m_Root);}
       auto         endForward()    {return forwardIterator();}
       auto         beginBackward() {return backwardIterator(this,&m_Root);}
       auto         endBackward()   {return backwardIterator();}
       
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
    auto forward() {return rangeView<forwardIterator>(beginForward(),endForward(),m_tree_mutex);}
    auto backward(){return rangeView<backwardIterator>(beginBackward(),endBackward(),m_tree_mutex);}

protected:
       BTNode          m_Root;
       size_t          m_Height;  // height of tree
       size_t          m_Order;   // order of tree
       size_t          m_NumKeys; // number of keys
       TF              m_Unique;  // Accept the elements only once ?
       recursive_mutex m_tree_mutex;
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
    this->backward().walk([&ss](const auto& node){
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