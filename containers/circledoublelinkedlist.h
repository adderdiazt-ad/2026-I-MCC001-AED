#ifndef CIRCLEDOUBLELINKEDLIST_H
#define CIRCLEDOUBLELINKEDLIST_H

#include "doublelinkedlist.h"
#include <mutex>
#include <utility>

template <typename T>
struct BaseCircleDoubleLinkedListTrait : public BaseContainerTrait<T, DLLNode<T>> {};

template <typename T>
struct AscendingCircleDoubleLinkedListTrait : public BaseCircleDoubleLinkedListTrait<T> {
    using Comp = std::less<T>;
};

template <typename T>
struct DescendingCircleDoubleLinkedListTrait : public BaseCircleDoubleLinkedListTrait<T> {
    using Comp = std::greater<T>;
};

template <typename Traits>
class CircleDoubleLinkedList : public DoubleLinkedList<Traits> {
public:
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using Comp       = typename Traits::Comp;
    using Parent     = DoubleLinkedList<Traits>;
    
    using forward_iterator = typename Parent::forward_iterator;
    using backward_iterator = typename Parent::backward_iterator;

public:
    CircleDoubleLinkedList() : Parent() {}
    CircleDoubleLinkedList(const CircleDoubleLinkedList &other);
    CircleDoubleLinkedList(CircleDoubleLinkedList &&other) noexcept : Parent(move(other)) {}
    ~CircleDoubleLinkedList() override ;

private:
    void breakCircularLinks();
    void fixCircularLinks();

public:
    virtual void insert(const value_type &value, Ref ref) override;

    virtual void push_front(value_type value, Ref ref) override;
    virtual void push_back(value_type value, Ref ref) override;
    virtual auto pop_front() -> std::pair<value_type, Ref> override;
    virtual auto pop_back() -> std::pair<value_type, Ref> override;

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
    void ReverseForEach(Func func, Args &&... args) {
        std::unique_lock<mutex> lock(this->m_mtx);
        auto it = this->rbegin();
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

    template <typename Func, typename... Args>
    backward_iterator ReverseFirstThat(Func func, Args &&... args) {
        std::unique_lock<mutex> lock(this->m_mtx);
        auto it = this->rbegin();
        for (std::size_t i = 0; i < this->m_size; ++i) {
            if (func((*it), std::forward<Args>(args)...)) {
                return it;
            }
            ++it;
        }
        return this->rend();
    }
};

template<typename Traits>
CircleDoubleLinkedList<Traits>::CircleDoubleLinkedList(const CircleDoubleLinkedList &other) : Parent() {
        scoped_lock<mutex> lock(other.m_mtx);
        Node* pTemp = other.m_pRoot;
        if (!pTemp) return;
        
        for (std::size_t i = 0; i < other.m_size; ++i) {
            this->push_back(pTemp->getData(), pTemp->getRef());
            pTemp = pTemp->getNext();
        }
    }
template<typename Traits>
CircleDoubleLinkedList<Traits>::~CircleDoubleLinkedList() {
        std::scoped_lock<mutex> lock(this->m_mtx);
        breakCircularLinks();
    }
template<typename Traits>
void CircleDoubleLinkedList<Traits>::insert(const value_type &value, Ref ref){
        {
            std::scoped_lock<mutex> lock(this->m_mtx);
            breakCircularLinks();
        }
        Parent::insert(value, ref);
        {
            std::scoped_lock<mutex> lock(this->m_mtx);
            fixCircularLinks();
        }
    }
template<typename Traits>
void CircleDoubleLinkedList<Traits>::breakCircularLinks() {
        if (this->m_pTail) this->m_pTail->setNext(nullptr);
        if (this->m_pRoot) this->m_pRoot->setPrev(nullptr);
    }
template<typename Traits>
void CircleDoubleLinkedList<Traits>::fixCircularLinks() {   
        if (this->m_pRoot && this->m_pTail) {
            this->m_pTail->setNext(this->m_pRoot);
            this->m_pRoot->setPrev(this->m_pTail);
        }
    } 
template<typename Traits>
void CircleDoubleLinkedList<Traits>::push_front(value_type value, Ref ref){
        {
            std::scoped_lock<mutex> lock(this->m_mtx);
            breakCircularLinks();
        }
        Parent::push_front(value, ref);
        {
            std::scoped_lock<mutex> lock(this->m_mtx);
            fixCircularLinks();
        }
    }   
template<typename Traits>
void CircleDoubleLinkedList<Traits>::push_back(value_type value, Ref ref){
        {
            std::scoped_lock<mutex> lock(this->m_mtx);
            breakCircularLinks();
        }
        Parent::push_back(value, ref);
        {
            std::scoped_lock<mutex> lock(this->m_mtx);
            fixCircularLinks();
        }
    }
template<typename Traits>
auto CircleDoubleLinkedList<Traits>::pop_front() -> std::pair<value_type, Ref>{
        {
            std::scoped_lock<mutex> lock(this->m_mtx);
            breakCircularLinks();
        }
        auto result = Parent::pop_front();
        {
            std::scoped_lock<mutex> lock(this->m_mtx);
            fixCircularLinks();
        }
        return result;
    }
template<typename Traits>
auto CircleDoubleLinkedList<Traits>::pop_back() -> std::pair<value_type, Ref>{
        {
            std::scoped_lock<mutex> lock(this->m_mtx);
            breakCircularLinks();
        }
        auto result = Parent::pop_back();
        {
            std::scoped_lock<mutex> lock(this->m_mtx);
            fixCircularLinks();
        }
        return result;
    }

#endif // CIRCLEDOUBLELINKEDLIST_H