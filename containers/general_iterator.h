#ifndef __ITERATOR_H__
#define __ITERATOR_H__
#include <algorithm>
#include <utility>
#include "../types.h"
using namespace std;

template <typename Container, class IteratorBase> // 
class general_iterator
{public:
    using Node   = typename Container::Node;
    using Myself = general_iterator<Container, IteratorBase>;
    
protected:
    Container *m_pContainer;
    Node      *m_pNode;
public:
    general_iterator(Container *pContainer, Node *pNode)
        : m_pContainer(pContainer), m_pNode(pNode) {}
    general_iterator(Myself &other) 
          : m_pContainer(other.m_pContainer), m_pNode(other.m_pNode){}
    general_iterator(Myself &&other) // Move constructor
          {   m_pContainer = move(other.m_pContainer);
              m_pNode      = move(other.m_pNode);
          }
    IteratorBase operator=(IteratorBase &iter)
          {   m_pContainer = move(iter.m_pContainer);
              m_pNode      = move(iter.m_pNode);
              return *(IteratorBase *)this; // Pending static_cast?
          }
    Node *getNode() const { return m_pNode; }
    friend bool operator==(const IteratorBase &a, const IteratorBase &b) { return a.getNode() == b.getNode(); } 
    friend bool operator!=(const IteratorBase &a, const IteratorBase &b) { return !(a == b); }
    Node &operator*(){ return *m_pNode; }
    Node* operator->() const { return m_pNode; }
};

template <typename T,typename derived_Node>
class general_node{
public:
    using value_type = T;
    using Node       = derived_Node;
    using myself     = general_node<T, derived_Node>;

protected:
    value_type      m_data;  
    Ref             m_ref;
    derived_Node    *m_pNext;
public:
    general_node(){}
    general_node(value_type data, Ref ref, derived_Node *pNext = nullptr) 
           : m_data(data), m_ref(ref), m_pNext(pNext) {}
    virtual ~general_node() {}

    value_type      getData() const { return m_data; }
    value_type&     getDataRef()    { return m_data; }
    void            setData(value_type data) { m_data = data; }
    Ref             getRef() const  { return m_ref; }
    Ref&            getRefRef()     { return m_ref; }
    void            setRef(Ref ref) { m_ref = ref; }
    
    derived_Node*   getNext() const { return m_pNext; }
    derived_Node*&  getNextRef()    { return m_pNext; }
    void            setNext(derived_Node *pNext) { m_pNext = pNext; }

    friend ostream &operator<<(ostream &os, const myself &node){
        return os << "(" << node.getData() << ", " << node.getRef() << ")";
    }
};

#endif
 