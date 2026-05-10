#ifndef DOUBLELINKEDLIST_H
#define DOUBLELINKEDLIST_H

#include "linkedlist.h"
#include "../foreach.h"

template <typename T>
class DLLNode :  public general_node<T, DLLNode<T>>{
public:
    using Parent = general_node<T, DLLNode<T>>;
    using Node       = DLLNode<T>;
private:
    Node *m_pPrev;
public:
    DLLNode() : Parent(), m_pPrev(nullptr) {}
    DLLNode(T data, Ref ref, Node* pNext = nullptr, Node* pPrev = nullptr) 
        : Parent(data, ref, pNext), m_pPrev(pPrev) {}
    Node*  getPrev() const { return m_pPrev; }
    Node*& getPrevRef() { return m_pPrev; }
    void   setPrev(Node* pPrev) { m_pPrev = pPrev; }
};

template <typename T>
struct BaseDoubleLinkedListTrait : public BaseContainerTrait<T, DLLNode<T>>{

};

template <typename T>
struct AscendingDoubleLinkedListTrait : public BaseDoubleLinkedListTrait<T>{
    using Comp = less<T>;
};

template <typename T>
struct DescendingDoubleLinkedListTrait : public BaseDoubleLinkedListTrait<T>{
    using Comp = greater<T>;
};

template <typename Container>
class DoubleLinkedListBackwardIterator : public general_iterator<Container, 
                                        DoubleLinkedListBackwardIterator<Container>>{
    using MySelf = DoubleLinkedListBackwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    MySelf& operator++(){
        this->m_pNode = this->m_pNode->getPrev();
        return *this;
    }
};

template <typename Traits>
class DoubleLinkedList : public LinkedList<Traits>{
public:
    using Parent     = LinkedList<Traits>;
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using Comp       = typename Traits::Comp;
    using MySelf     = DoubleLinkedList<Traits>;

    using forward_iterator  = LinkedListForwardIterator<MySelf>;
    using backward_iterator = DoubleLinkedListBackwardIterator<MySelf>;

public:
    DoubleLinkedList() : Parent() {}
    DoubleLinkedList(DoubleLinkedList &other);//constructor de copia
    DoubleLinkedList(DoubleLinkedList &&other){
        scoped_lock<mutex> lock(this->m_mtx);
        this->m_pRoot = exchange(other.m_pRoot, nullptr);
        this->m_pTail = exchange(other.m_pTail, nullptr);
        this->m_size = exchange(other.m_size, 0);
    }
    virtual ~DoubleLinkedList()override {}
    
    size_t size () const { return this->m_size; }
    bool isEmpty() const { return this->m_pRoot == nullptr; }
    
    public:
    
    virtual void insert(const value_type &value, Ref ref) override {
        scoped_lock<mutex> lock(this->m_mtx);
        auto [pPrev, pCurrent] = this->find_position(value);
        Node* pNew = new Node(value, ref, pCurrent, pPrev);
        if (pPrev) 
            pPrev->setNext(pNew);
        else 
            this->m_pRoot = pNew; 
        if (pCurrent) 
            pCurrent->setPrev(pNew);
        else 
            this->m_pTail = pNew; 
        this->m_size++;
    }
    virtual void push_front(value_type value, Ref ref) override;
    virtual void push_back(value_type value, Ref ref)override;
    virtual auto pop_front() -> pair<value_type, Ref>override;
    virtual auto pop_back() -> pair<value_type, Ref>override;

    forward_iterator begin()   { return forward_iterator(this, this->m_pRoot); }
    forward_iterator end()     { return forward_iterator(this, nullptr); }
    backward_iterator rbegin() { return backward_iterator(this, this->m_pTail); }
    backward_iterator rend()   { return backward_iterator(this, nullptr); }

    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&... args){
        scoped_lock<mutex> lock(this->m_mtx);
        ::ForEach(begin(), end(), func, forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void ReverseForEach(Func func, Args &&... args){
        scoped_lock<mutex> lock(this->m_mtx);
        ::ForEach(rbegin(), rend(), func, forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args &&... args){
        scoped_lock<mutex> lock(this->m_mtx);
        return ::FirstThat(begin(), end(), func, forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    backward_iterator ReverseFirstThat(Func func, Args &&... args){
        scoped_lock<mutex> lock(this->m_mtx);
        return ::FirstThat(rbegin(), rend(), func, forward<Args>(args)...);
    }
};

template <typename Traits>
DoubleLinkedList<Traits>::DoubleLinkedList(DoubleLinkedList &other){
    scoped_lock<mutex> lock(other.m_mtx);
    Node* pTemp = other.m_pRoot;
    while(pTemp != nullptr){
        push_back(pTemp -> getData(), pTemp -> getRef());
        pTemp = pTemp->getNext();
    }
}
template <typename Traits>
void DoubleLinkedList<Traits>::push_front(value_type value, Ref ref){
    scoped_lock<mutex> lock(this->m_mtx);
    Node* ptemp = new Node(value, ref, this->m_pRoot, nullptr);
    if (this->m_pRoot)
        this->m_pRoot->setPrev(ptemp);
    this->m_pRoot = ptemp;
    if (this->m_size==0)
        this->m_pTail = ptemp;
    ++this->m_size;
}

template <typename Traits>
void DoubleLinkedList<Traits>::push_back(value_type value, Ref ref){
    scoped_lock<mutex> lock(this->m_mtx);
    Node* ptemp = new Node(value, ref, nullptr, this->m_pTail);
    if (this->m_size==0){
        this->m_pRoot = ptemp;
        this->m_pTail = ptemp;
    } else {
        this->m_pTail->setNext(ptemp);
        this->m_pTail = ptemp;
    }
    ++this->m_size;
}

template <typename Traits>
auto DoubleLinkedList<Traits>::pop_front() -> pair<value_type, Ref>{
    auto result = Parent::pop_front();
    scoped_lock<mutex> lock(this->m_mtx);
    if (this->m_pRoot)
        this->m_pRoot->setPrev(nullptr);
    return result;
}

template <typename Traits>
auto DoubleLinkedList<Traits>::pop_back() -> pair<value_type, Ref>{
    scoped_lock<mutex> lock(this->m_mtx);
    if (!this->m_pTail)
        throw out_of_range("pop_back(): empty list");
    Node* pTemp = this->m_pTail;
    auto result = make_pair(pTemp->getData(), pTemp->getRef());
    if(this->m_pTail==this->m_pRoot){
        this->m_pTail = nullptr;
        this->m_pRoot = nullptr;
    } else {
        this->m_pTail = this->m_pTail->getPrev();
        this->m_pTail->setNext(nullptr);
    }
    delete pTemp;
    --this->m_size;
    return result;
}
#endif