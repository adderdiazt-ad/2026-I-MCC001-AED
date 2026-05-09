#ifndef CIRCLELINKEDLIST_H
#define CIRCLELINKEDLIST_H
#include "linkedlist.h"
#include <mutex>
#include <utility>

template <typename T>
struct BaseCircleLinkedListTrait : public BaseContainerTrait<T, LLNode<T>> {};

template <typename T>
struct AscendingCircleLinkedListTrait : public BaseCircleLinkedListTrait<T> {
    using Comp = std::less<T>;
};

template <typename T>
struct DescendingCircleLinkedListTrait : public BaseCircleLinkedListTrait<T> {
    using Comp = std::greater<T>;
};

template <typename Traits>
class CircleLinkedList : public LinkedList<Traits>{
public:
    using Parent     = LinkedList<Traits>;
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using Comp       = typename Traits::Comp;  

    using forward_iterator = typename Parent::forward_iterator;

public:
    CircleLinkedList() : Parent() {}
    CircleLinkedList(const CircleLinkedList &other);
    CircleLinkedList(CircleLinkedList &&other) noexcept : Parent(move(other)) {}
    ~CircleLinkedList() override;
    virtual void push_front(value_type value, Ref ref) override;
    virtual void push_back(value_type value, Ref ref) override;
    virtual auto pop_front() -> pair<value_type, Ref> override;
    virtual auto pop_back() -> pair<value_type, Ref> override;
    virtual void insert(const value_type &value, Ref ref) override;

    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&... args) {
        std::unique_lock<mutex> lock(this->m_mtx);
        auto it = this->begin();
        for (std::size_t i = 0; i < this->m_size; ++i) {
            func((*it), std::forward<Args>(args)...); 
            ++it; 
        }
    }

    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args &&... args) {
        std::unique_lock<mutex> lock(this->m_mtx);
        auto it = this->begin(); 
        for (std::size_t i = 0; i < this->m_size; ++i) {
            if (func((*it), std::forward<Args>(args)...)) {
                return it; 
            }
            ++it;
        }
        return this->end(); 
    }

};
template <typename Traits>
CircleLinkedList<Traits>::CircleLinkedList(const CircleLinkedList &other) : Parent() {
    scoped_lock<mutex> lock(other.m_mtx);
    Node* pTemp = other.m_pRoot;
    if (!pTemp) return;
    for (size_t i = 0; i < other.m_size; ++i) {
        this->push_back(pTemp->getData(), pTemp->getRef());
        pTemp = pTemp->getNext();
    }
}
template <typename Traits>
CircleLinkedList<Traits>::~CircleLinkedList() {
    scoped_lock<mutex> lock(this->m_mtx);
    if (this->m_pTail)
        this->m_pTail->setNext(nullptr);
}
template <typename Traits>
void CircleLinkedList<Traits>::insert(const value_type &value, Ref ref){
        {
            scoped_lock<mutex> lock(this->m_mtx);
            if(this->m_pTail) this->m_pTail->setNext(nullptr);
        }
        Parent::insert(value, ref);
        {
            scoped_lock<mutex> lock(this->m_mtx);
            if (this->m_pTail) this->m_pTail->setNext(this->m_pRoot);
        }
}
template <typename Traits>
void CircleLinkedList<Traits>::push_front(value_type value, Ref ref){
        Parent::push_front(value, ref);
        scoped_lock<mutex> lock(this->m_mtx);
        if (this->m_pTail)
            this->m_pTail->setNext(this->m_pRoot);
    }
template <typename Traits>
void CircleLinkedList<Traits>::push_back(value_type value, Ref ref){
        Parent::push_back(value, ref);
        scoped_lock<mutex> lock(this->m_mtx);
        if (this->m_pTail)
            this->m_pTail->setNext(this->m_pRoot);
    }
template <typename Traits>
auto CircleLinkedList<Traits>::pop_front() -> pair<value_type, Ref>{
        auto result = Parent::pop_front();
        scoped_lock<mutex> lock(this->m_mtx);
        if (this->m_pTail)
            this->m_pTail->setNext(this->m_pRoot);
        return result;
}

template <typename Traits>
auto CircleLinkedList<Traits>::pop_back() -> pair<value_type, Ref>{
        {
            scoped_lock<mutex> lock(this->m_mtx);
            if (this->m_pTail) this->m_pTail->setNext(nullptr);
        }
        auto result = Parent::pop_back();
        {
            scoped_lock<mutex> lock(this->m_mtx);
            if (this->m_pTail) this->m_pTail->setNext(this->m_pRoot);
        }
        return result;
}

#endif