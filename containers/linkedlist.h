#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <iostream>
#include <cstddef>   // size_t
#include <string>
#include <sstream>
#include<utility>    // move, exchange
#include <mutex>     // mutex
#include "general_iterator.h"
#include "../util.h"
#include "../types.h"
#include "../foreach.h"
#include "basetrait.h"

using namespace std;

// Forward iterator
template <typename Container>
class LinkedListForwardIterator : public general_iterator<Container, 
                                            LinkedListForwardIterator<Container>>{
    using MySelf = LinkedListForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    MySelf& operator++(){
        this->m_pNode = this->m_pNode->getNext();
        return *this;
    }
};

// Linked List Node
template <typename T>
class LLNode: public general_node<T, LLNode<T>>{
public:
    using Parent = general_node<T, LLNode<T>>;
    using Parent::Parent;
};


template <typename T>
struct BaseLinkedListTrait : public BaseContainerTrait<T, LLNode<T>>{

};

template <typename T>
struct AscendingLinkedListTrait : public BaseLinkedListTrait<T>{
    using Comp = less<T>;
};

template <typename T>
struct DescendingLinkedListTrait : public BaseLinkedListTrait<T>{
    using Comp = greater<T>;
};

template <typename Traits>
class LinkedList{
public:
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using Comp       = typename Traits::Comp;
    using MySelf     = LinkedList<Traits>;

    using forward_iterator = LinkedListForwardIterator<MySelf>;
    // friend forward_iterator;

protected:
    Node *m_pRoot = nullptr;
    Node *m_pTail = nullptr;
    size_t m_size = 0;
    Comp   m_comp;
    mutable mutex m_mtx;
public:
    LinkedList() {}
    LinkedList(const LinkedList &other){ // Copy constructor
        scoped_lock<mutex> lock(other.m_mtx);
        Node* pTemp = other.m_pRoot;

        while(pTemp != nullptr){
            push_back(pTemp -> getData(), pTemp -> getRef());
            pTemp = pTemp->getNext();
        }
            
    }
    LinkedList(LinkedList &&other){ // Move constructor
        
        scoped_lock<mutex> lock(m_mtx);
        m_pRoot = exchange(other.m_pRoot, nullptr);
        m_pTail = exchange(other.m_pTail, nullptr);
        m_size = exchange(other.m_size, 0);

    }
    //LinkedList& operator=(const LinkedList &other){ // Copy assignment operator
   // //}
    //LinkedList& operator=(LinkedList &&other){ // Move assignment operator
    //}
    
    virtual        ~LinkedList() {
        
        scoped_lock<mutex> lock(m_mtx);
        Node* pTemp = m_pRoot;
        while (pTemp){
            Node* pNext = pTemp->getNext();
            delete pTemp;
            pTemp = pNext;
        }

        m_pRoot = nullptr;
        m_pTail = nullptr;
        m_size  = 0;
    }

    virtual void   push_front(value_type value, Ref ref) {
        scoped_lock<mutex> lock(m_mtx);
        Node* pTemp = new Node(value, ref, m_pRoot);  //Se crea el Nodo temporal con los datos ingresados que apunta a m_pRoot
        m_pRoot = pTemp;                              //Se actualiza el nodo raiz
        if (m_size == 0) 
            m_pTail = pTemp;                          //Si la lista esta vacia la cola tambien se debe actualizar
        ++m_size;
    }
    virtual auto    pop_front() -> pair<value_type, Ref>{ 
        scoped_lock<mutex> lock(m_mtx);
        if( m_pRoot ){
            Node* pTemp = m_pRoot;
            m_pRoot = m_pRoot->getNext();
            --m_size;
            auto result = make_pair(pTemp->getData(), pTemp->getRef());
            if (!m_pRoot) 
                m_pTail = nullptr;
            delete pTemp;
            return result;
        }else
            throw out_of_range("pop_front(): empty list");
    }
    virtual void    push_back(value_type value, Ref ref){
        scoped_lock<mutex> lock(m_mtx);
        Node* pTemp = new Node(value, ref, nullptr);  //Como es el ultimo nodo no apunta a nada
        if (m_size == 0){
            m_pRoot = pTemp;
            m_pTail = pTemp; 
        } else {
            m_pTail->setNext(pTemp);
            m_pTail = pTemp;
        }                  
        ++m_size;
    }
    virtual auto    pop_back() -> pair<value_type, Ref>{
        scoped_lock<mutex> lock(m_mtx);
        if( !m_pRoot )
            throw out_of_range("pop_back(): empty list");
        
        //Con un solo elemento
        if( m_pRoot == m_pTail){
            auto pDelete = make_pair(m_pTail->getData(), m_pTail->getRef());

            delete m_pTail;
            m_pRoot = nullptr;
            m_pTail = nullptr;
            
            --m_size;
            return pDelete;
        }

        //Lista con varios elementos
        Node *pTemp = m_pRoot;

        while (pTemp->getNext() != m_pTail)             //Recorrer toda la lista hasta el penultimo elemento
            pTemp = pTemp->getNext();

        auto pDelete = make_pair(m_pTail->getData(), m_pTail->getRef());
        
        delete pTemp->getNext();
        pTemp->setNext(nullptr);

        m_pTail = pTemp;
        --m_size;
        return pDelete;
    }
protected:
    virtual auto find_position(const value_type &value) -> pair<Node*, Node*>;
public:
    virtual void    insert(const value_type &value, Ref ref);
    
    virtual Node& operator[](const size_t index) const{
        
        if (index >= m_size)
        throw out_of_range("Index out of range");

        Node* pTemp = m_pRoot;
        for (size_t i = 0; i < index; ++i){
            pTemp = pTemp -> getNext();
        }
        return *pTemp;
    };

    virtual size_t  size() const { return m_size; }
    virtual string  toString();

    forward_iterator begin() { return forward_iterator(this, m_pRoot); }
    forward_iterator end()   { return forward_iterator(this, nullptr); }

    // Agregar Foreach
    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&...  args){
        unique_lock<mutex> lock(m_mtx);
        ::ForEach(begin(), end(), func, std::forward<Args>(args)... );
    }

    //Agregar FirstThat
    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args &&...  args){
        return ::FirstThat(begin(), end(), func, std::forward<Args>(args)... );
    }
};

template <typename Traits>
 auto LinkedList<Traits>::find_position(const value_type &value) -> pair<Node*, Node*> {
        Node* pCurrent = m_pRoot;
        Node* pPrev = nullptr;
        while (pCurrent && !m_comp(value, pCurrent->getDataRef())) {
            pPrev = pCurrent;
            pCurrent = pCurrent->getNext();
        }
        return {pPrev, pCurrent};
    }

template <typename Traits>
void LinkedList<Traits>::insert(const value_type &value, Ref ref){
    scoped_lock<mutex> lock(m_mtx);
    auto [pPrev, pCurrent] = find_position(value);
    Node* pNew = new Node(value, ref, pCurrent);
    if (pPrev) 
        pPrev->setNext(pNew);
    else 
         m_pRoot = pNew; 
    if (!pCurrent) 
            m_pTail = pNew;
     m_size++;
    }

template <typename Traits>
string  LinkedList<Traits>::toString() {
    stringstream ss;
    Node *pNode = m_pRoot;
    ss << "[";
    if( m_size > 0 ){
        for( size_t i = 0 ; i < size()-1 ; ++i ){
            ss << *pNode << ",";
            pNode = pNode->getNext();
        }
        ss << *pNode;
    }
    ss << "]";
    return ss.str();
}

template <typename Traits>
ostream& operator<<(ostream& os, LinkedList<Traits>& list){
    return os << list.toString();
}

template <typename Traits>
istream& operator>>(istream& is, LinkedList<Traits>& list){
    using value_type = typename LinkedList<Traits>::value_type;
    string line;
    getline(is, line);
    for (char& c : line){
        if (c == '[' || c == ']' || c == '(' || c == ')' || c == ',')
            c = ' ';
    }
    value_type value;
    Ref ref;
    stringstream ss(line);
    while (ss >> value >> ref){
        list.insert(value, ref);
    }

    return is;
}

#endif // __LINKEDLIST_H__