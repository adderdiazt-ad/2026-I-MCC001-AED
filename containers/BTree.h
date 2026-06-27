// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <cstddef>
#include <functional>
#include "BTreePage.h"
#include "../types.h"       

#define DEFAULT_BTREE_ORDER 3

template <typename Traits>
class BTree 
// this is the full version of the BTree
{
       /*struct Node
       {
               keyType first;
               L    second;
               Node *&operator->() { return this; }
       };*/

public:
       //typedef Node iterator;
       using BTNode         =      CBTreePage<Traits>;// useful shorthand
       using keyType        =      typename Traits::value_type;
       using ObjIDType      =      typename Traits::ObjIDType;
       using Node           =      typename BTNode::Node;

public:
       BTree(size_t order = DEFAULT_BTREE_ORDER, TF unique = true);
       ~BTree();
       //int           Open (char * name, int mode);
       //int           Create (char * name, int mode);
       //int           Close ();
       TF            Insert (const keyType key, const ObjIDType ObjID);
       TF            Remove (const keyType key, const ObjIDType ObjID);
       ObjIDType     Search (const keyType key);
       size_t        size()  { return m_NumKeys; }
       size_t        height() { return m_Height;      }
       size_t        GetOrder() { return m_Order;     }

       void            Print (ostream &os);
       template <typename Func, typename... Args>
       void ForEach(Func func, Args&&... args);
       template <typename Func, typename... Args>
       Node* FirstThat(Func func, Args&&... args);
       //typedef               Node iterator;

protected:
       BTNode          m_Root;
       size_t          m_Height;  // height of tree
       size_t          m_Order;   // order of tree
       size_t          m_NumKeys; // number of keys
       TF              m_Unique;  // Accept the elements only once ?
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
{
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
{
       bt_ErrorCode error = m_Root.Remove(key, ObjID);
       if( error == bt_duplicate || error == bt_nofound )
               return false;
       m_NumKeys--;

       if( error == bt_rootmerged )
               m_Height--;
       return true;
}

template <typename Traits>
typename BTree<Traits>::ObjIDType
BTree<Traits>::Search (const keyType key)
{
       ObjIDType ObjID = -1;
       m_Root.Search(key, ObjID);
       return ObjID;
}


template <typename Trait>
template <typename Func, typename... Args>
void BTree<Trait>::ForEach(Func func, Args&&... args){
    m_Root.ForEach(0, func, std::forward<Args>(args)...);
}

template <typename Traits>
template <typename Func, typename... Args>
typename BTree<Traits>::Node *
BTree<Traits>::FirstThat(Func func, Args&&... args)
{
       return m_Root.FirstThat(0, func, std::forward<Args>(args)... );
}


template <typename Traits>
void BTree<Traits>::Print(ostream &os){
       m_Root.Print(os);
}






#endif