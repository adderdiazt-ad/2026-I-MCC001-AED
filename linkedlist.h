#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <iostream>
#include <cstddef>   // size_t
#include <string>
#include <sstream>
#include <mutex>     // mutex
#include "general_iterator.h"
#include "util.h"
#include "types.h"
#include "foreach.h"
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
    // TODO: Completar el operator++
    MySelf& operator++(){
        this->m_pNode = this->m_pNode->getNext();
        return *this;
    }
};

// Linked List Node
template <typename T>
class LLNode{
    using value_type = T;
    using Node       = LLNode<T>;
private:
    value_type m_data;
    Ref        m_ref;
    Node      *m_pNext;
public:
    LLNode(){}
    LLNode(value_type data, Ref ref, Node *pNext = nullptr) 
           : m_data(data), m_ref(ref), m_pNext(pNext) {}
    virtual ~LLNode() {}

    value_type      getData() const { return m_data; }
    value_type&     getDataRef()    { return m_data; }
    void            setData(value_type data) { m_data = data; }
    Ref             getRef() const  { return m_ref; }
    Ref&            getRefRef()     { return m_ref; }
    void            setRef(Ref ref) { m_ref = ref; }
    Node*           getNext() const { return m_pNext; }
    Node*&          getNextRef()    { return m_pNext; }
    void            setNext(Node *pNext) { m_pNext = pNext; }
};

template <typename T>
ostream &operator<<(ostream &os, const LLNode<T> &node){
    return os << "(" <<node.getData() << ", " << node.getRef() << ")";
}

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

private:
    Node *m_pRoot = nullptr;
    Node *m_pTail = nullptr;
    size_t m_size = 0;
    Comp   m_comp;
    mutable mutex m_mtx;
public:
    LinkedList() {}
    LinkedList(const LinkedList &other); // Copy constructor
    LinkedList(LinkedList &&other) noexcept; // Move constructor
    
    LinkedList& operator=(const LinkedList &other){ // Copy assignment operator
    }
    LinkedList& operator=(LinkedList &&other){ // Move assignment operator
    }
    
    virtual        ~LinkedList();
    virtual void    push_front(value_type value, Ref ref);
    virtual auto    pop_front() -> std::pair<value_type, Ref>;
    virtual void    push_back(value_type value, Ref ref);
    virtual auto    pop_back() -> std::pair<value_type, Ref>;
private:
            void    internal_insert(Node* &pParent, const value_type &value, Ref ref);
public:
    virtual void    insert(const value_type &value, Ref ref);
    virtual size_t  size() const { return m_size; }
    virtual string  toString();

    forward_iterator begin() { return forward_iterator(this, m_pRoot); }
    forward_iterator end()   { return forward_iterator(this, nullptr); }

    const Node& operator[](size_t index) const {
        if (index >= size() ||m_pRoot == nullptr) {
            throw std::out_of_range("Index out of range");
        }
        Node* pAux = m_pRoot;
        for (size_t i = 0; i < index; ++i) {
            pAux = pAux->getNext();
        }
        return *pAux;
    }

    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&...  args){
        unique_lock<mutex> lock(m_mtx);
        ::ForEach(begin(), end(), func, std::forward<Args>(args)... );
    }

    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args &&... args) {
        unique_lock<mutex> lock(m_mtx);
        return ::FirstThat(begin(), end(), func, std::forward<Args>(args)...);
    }
};

template <typename Traits>
void LinkedList<Traits>::internal_insert(Node* &pPrev, const value_type &value, Ref ref){
    if(!pPrev || m_comp(value, pPrev->getDataRef())){
        pPrev = new Node(value, ref, pPrev);
        m_size++;
        if(pPrev->getNext() == nullptr)
            m_pTail = pPrev;
        return;
    }
    internal_insert(pPrev->getNextRef(), value, ref);
    if(pPrev->getNext() == nullptr)
            m_pTail = pPrev;
}

template <typename Traits>
void LinkedList<Traits>::insert(const value_type &value, Ref ref){
    lock_guard<mutex> lock(m_mtx);
    internal_insert(m_pRoot, value, ref);
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
std::istream& operator>>(std::istream& is, LinkedList<Traits>& list) {
    typename LinkedList<Traits>::value_type value;
    Ref ref; 
    if (is >> value >> ref) 
        list.insert(value, ref);
    return is;
}

template <typename Traits>
 LinkedList<Traits>::LinkedList(const LinkedList &other) // Copy constructor
 : m_size(0), m_pRoot(nullptr), m_pTail(nullptr), m_comp(other.m_comp) { 
        std::lock_guard<mutex> lock(other.m_mtx);
        if (!other.m_pRoot) return;
        Node* pAux = other.m_pRoot;
        while (pAux != nullptr) {
            Node* newNode = new Node(pAux->getData(), pAux->getRef(), nullptr);
            if (!m_pRoot) {//primer nodo
                m_pRoot = newNode;
                m_pTail = newNode;
            } else {//el resto de nodos
                m_pTail->setNext(newNode);
                m_pTail = newNode;
            }
            m_size++;
            pAux = pAux->getNext();
        }
    }

template <typename Traits>
LinkedList<Traits>::LinkedList(LinkedList &&other) noexcept { // Move constructor
    std::lock_guard<mutex> lock(other.m_mtx); 

    this->m_pRoot = exchange(other.m_pRoot, nullptr);
    this->m_pTail = exchange(other.m_pTail, nullptr);
    this->m_size  = exchange(other.m_size, 0); 
    this->m_comp  = move(other.m_comp);
}
template <typename Traits>
LinkedList<Traits>::~LinkedList() {//destructor
    lock_guard<mutex> lock(m_mtx);
    Node* pAux = m_pRoot;
    while (pAux != nullptr) {
        Node* next = pAux->getNext(); 
        delete pAux;                  
        pAux = next;              
    }
    m_pRoot = nullptr;
    m_pTail = nullptr;
    m_size = 0;
}


template <typename Traits>
void LinkedList<Traits>::push_front(value_type value, Ref ref){
        lock_guard<mutex> lock(m_mtx);
        Node* pNewNode = new Node(value, ref, m_pRoot);
        m_pRoot = pNewNode;
        if( m_size == 0 )
            m_pTail = pNewNode;
        m_size++;
    }

template <typename Traits>
void LinkedList<Traits>::push_back(value_type value, Ref ref){
        lock_guard<mutex> lock(m_mtx);
        Node* pNewNode = new Node(value, ref);
        if( !m_pRoot )
            m_pRoot = pNewNode;
        else
            m_pTail->setNext(pNewNode);
        m_pTail = pNewNode;
        m_size++;
    }

template <typename Traits>
auto LinkedList<Traits>::pop_front() -> std::pair<value_type, Ref>{ 
        lock_guard<mutex> lock(m_mtx);
        if( m_pRoot ){
            Node* pTemp = m_pRoot;
            m_pRoot = m_pRoot->getNext();
            auto result = std::make_pair(pTemp->getData(), pTemp->getRef());
            m_size--;
            if (m_size == 0) {
            m_pTail = nullptr;
            }
            delete pTemp;
            return result;
        }else
            throw std::out_of_range("pop_front(): empty list");
    }

template <typename Traits>
auto LinkedList<Traits>::pop_back() -> std::pair<value_type, Ref>{
        lock_guard<mutex> lock(m_mtx);
        if(!m_pRoot)
            throw std::out_of_range("pop_back(): empty list");
        Node* pTemp = m_pTail;
        auto result = std::make_pair(pTemp->getData(), pTemp->getRef());
        if(m_pRoot != m_pTail){
            Node* pAux = m_pRoot;
            while( pAux->getNext() != m_pTail)
                pAux = pAux->getNext();
            pAux->setNext(nullptr);
            m_pTail = pAux;
        }else{
            m_pRoot = nullptr;
            m_pTail = nullptr;
        }
        m_size--;
        delete pTemp;
        return result;
    }


#endif // __LINKEDLIST_H__