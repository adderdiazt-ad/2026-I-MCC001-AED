#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <cstddef>
#include <sstream> 
#include <mutex>
#include "general_iterator.h"
#include "../types.h"
#include "../foreach.h"
#include "basetrait.h"

template <typename Container>
class vector_forward_iterator : public general_iterator<Container, vector_forward_iterator<Container>> {
public:
    using MySelf = vector_forward_iterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
    MySelf operator++() { this->m_pNode++; return *this; }
};

template <typename Container>
class vector_backward_iterator : public general_iterator<Container, vector_backward_iterator<Container>> {
public:
    using MySelf = vector_backward_iterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
    MySelf operator++() { this->m_pNode--; return *this; }
};

template <typename T>
struct VectorNode{
    using value_type = T;

    value_type   m_data;
    Ref          m_ref;
    VectorNode() : m_data(value_type()), m_ref(Ref()) {}
    VectorNode(value_type data, Ref ref) : m_data(data), m_ref(ref) {}
    value_type   GetData() const { return m_data; }
    value_type&  GetDataRef()    { return m_data; }
    Ref GetRef()  const { return m_ref;  }
    void operator++() { ++m_data; }
    void operator+=(const value_type& value) { m_data += value; }
    friend ostream &operator<<(ostream &os, const VectorNode<value_type> &Node){
        return os<< "(" << Node.GetData() << ", " << Node.GetRef() << ")";
    }
};

template <typename T>
struct VectorTraits : public BaseContainerTrait<T, VectorNode<T>>{
};

template <typename Traits>
class Vector{
    public:
        using value_type = typename Traits::value_type;
        using Node       = typename Traits::Node;
        using MySelf     = Vector<Traits>;
        using forward_iterator  = vector_forward_iterator<MySelf>;
        using backward_iterator = vector_backward_iterator<MySelf>;
    private:
        Node * m_data;
        size_t m_size;
        size_t m_capacity;
        mutex  m_mtx;
    private:
        void resize();
    public:
        Vector(size_t capacity = 10);
        ~Vector();
        void push_back(value_type value, Ref ref);

        size_t size();
        string ToString();

        forward_iterator  begin()   { return forward_iterator(this, m_data); }
        forward_iterator  end()     { return forward_iterator(this, m_data + m_size); }
        backward_iterator rbegin()  { return backward_iterator(this, m_data + m_size - 1); }
        backward_iterator rend()    { return backward_iterator(this, m_data - 1); }
        
        template <typename Func, typename... Args>
        void ForEach(Func func, Args &&... args){
            ::ForEach(begin(), end(), func, forward<Args>(args)...);
        }

        template <typename Func, typename... Args>
        void ReverseForEach(Func func, Args &&... args){
            ::ForEach(rbegin(), rend(), func, forward<Args>(args)...);
        }

        template <typename Func, typename... Args>
        forward_iterator FirstThat(Func func, Args &&... args){
            return ::FirstThat(begin(), end(), func, forward<Args>(args)...);
        }

        template <typename Func, typename... Args>
        backward_iterator ReverseFirstThat(Func func, Args &&... args){
            return ::FirstThat(rbegin(), rend(), func, forward<Args>(args)...);
        }
};

template <typename Traits>
Vector<Traits>::Vector(size_t capacity) {
    m_data = nullptr;
    m_size = 0;
    m_capacity = capacity;
    m_data = new Node[m_capacity];
}

template <typename Traits>
Vector<Traits>::~Vector() {
    delete[] m_data;
}

template <typename Traits>
void Vector<Traits>::push_back(value_type value, Ref ref) {
    if (m_size == m_capacity)
        resize();
    scoped_lock<mutex> lock(m_mtx);
    m_data[m_size] = Node(value, ref);
    m_size++;
}

template <typename Traits>
void Vector<Traits>::resize() {
    scoped_lock<mutex> lock(m_mtx);
    m_capacity = m_capacity < 10 ? 10: m_capacity * 2;
    Node *newData = new Node[m_capacity];
    for (size_t i = 0; i < m_size; i++)
        newData[i] = m_data[i];
    delete[] m_data;
    m_data = newData;
}

template <typename Traits>
size_t Vector<Traits>::size() {
    return m_size;
}

// [1, 2, 3, 4, 5]
template <typename Traits>
string Vector<Traits>::ToString() {
    scoped_lock lock(m_mtx);
    ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < m_size-1; i++)
        oss << m_data[i] << ",";
    if (m_size > 0)
        oss << m_data[m_size-1];
    oss << "]";
    return oss.str();
}

template <typename Traits>
ostream& operator<<(ostream& os, Vector<Traits>& v){
    return os << v.ToString();
}

void DemoVector();
void DemoConcurrentVector();

#endif // __VECTOR_H__