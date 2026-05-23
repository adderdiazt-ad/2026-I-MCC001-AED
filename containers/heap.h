#ifndef __HEAP_H__
#define __HEAP_H__
#include <vector>
#include <mutex>
#include "vector.h"


using namespace std;

template <typename T>
class HeapNode{
    public:
        using value_type = T;
    private:
        value_type   m_data;
        Ref          m_ref;
    public:
        HeapNode(value_type data, Ref ref) : m_data(data), m_ref(ref) {}
        
        value_type      GetData() const { return m_data; }
        value_type&     GetDataRef()    { return m_data; }
        Ref             GetRef()  const { return m_ref;  }
        Ref&            GetRefRef()     { return m_ref; }
        friend ostream &operator<<(ostream &os, const HeapNode<value_type> &node){
            return os << "(" << node.GetData() << ", " << node.GetRef() << ")";
        }
};

template <typename T>
struct AscendingHeapTrait : public BaseContainerTrait<T, HeapNode<T> >,
                            public AscendingTrait<T>
{
};

template <typename T>
struct DescendingHeapTrait : public BaseContainerTrait<T, HeapNode<T> >,
                            public DescendingTrait<T>
{
};

// Revisar: https://www.cs.usfca.edu/~galles/visualization/Heap.html
// Pero en este ejercicio empezamos el la posicion [0]
template <typename Traits>
class Heap {
public:
    using value_type        = typename Traits::value_type;
    using Node              = typename Traits::Node;
    using Comp              = typename Traits::Comp;
    using forward_iterator  = vector_forward_iterator<Heap<Traits>>;
    using backward_iterator = vector_backward_iterator<Heap<Traits>>;
    using MySelf            = Heap<Traits>;
private:
    std::vector<Node>   m_heap;
    Comp                m_comp;
    mutable mutex       m_mtx;
public:
        forward_iterator  begin()   { return forward_iterator(this, m_heap.data()); }
        forward_iterator  end()     { return forward_iterator(this, m_heap.data() + m_heap.size()); }
        backward_iterator rbegin()  { return backward_iterator(this, m_heap.data() + m_heap.size() - 1); }
        backward_iterator rend()    { return backward_iterator(this, m_heap.data() - 1); }

public:
    Heap() : m_heap(), m_comp() {}
    Heap(const std::vector<std::pair<value_type, Ref>>& elements) {
        build_heap(elements);
    }
    Heap(const Heap& other) {
        scoped_lock<mutex> lock(other.m_mtx);
        m_heap = other.m_heap;
        m_comp = other.m_comp;
    }
    Heap(Heap&& other) {
        scoped_lock<mutex> lock(other.m_mtx);
        m_heap = move(other.m_heap);
        m_comp = move(other.m_comp);
    }
    // revisar como trabaja el agotirmo de Floyd para construir el heap a partir de un vector de elementos
    void build_heap(const vector<pair<value_type, Ref>>& elements);
    void insert(const value_type &value, Ref ref);
    auto extract()-> std::pair<value_type, Ref>;
    bool empty()    const { scoped_lock<mutex> lock(m_mtx); return m_heap.empty(); }
    size_t size()   const { scoped_lock<mutex> lock(m_mtx); return m_heap.size(); }
    istream& read(istream& is);
    string to_string() const;
    value_type peek_root() const{
        scoped_lock<mutex> lock(m_mtx);
        if (m_heap.empty()) 
            throw std::out_of_range("Heap is empty");
        return m_heap[0].GetData();
    }
private:
    void heapify_up(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if ( m_comp(m_heap[index].GetData(), m_heap[parent].GetData()) ) {
                std::swap(m_heap[index], m_heap[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    void heapify_down(size_t index) {
        while (true) {
            size_t left = 2 * index + 1;
            size_t right = 2 * index + 2;
            size_t extreme = index;
            if (left < m_heap.size() && m_comp(m_heap[left].GetData(), m_heap[extreme].GetData()))
                extreme = left;
            if (right < m_heap.size() && m_comp(m_heap[right].GetData(), m_heap[extreme].GetData()))
                extreme = right;
            if (extreme == index) 
                break;
            std::swap(m_heap[index], m_heap[extreme]);
            index = extreme;
        }
    }

public: 
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
void Heap<Traits>::build_heap(const vector<pair<value_type, Ref>>& elements){
        scoped_lock<std::mutex> lock(m_mtx);
        m_heap.clear();
        m_heap.reserve(elements.size());
        for (const auto& item : elements) {
            m_heap.push_back(Node(item.first, item.second));
        }
        if (m_heap.size() > 1) {
            for (size_t i = m_heap.size() / 2; i > 0; --i) {
                heapify_down(i - 1);
            }
        }
}
template <typename Traits>
void Heap<Traits>::insert(const value_type &value, Ref ref){
    scoped_lock<mutex> lock(m_mtx);
    m_heap.push_back( Node(value, ref) );
    heapify_up(m_heap.size() - 1);
}
template <typename Traits>
auto Heap<Traits>::extract() -> std::pair<value_type, Ref> {
    scoped_lock<mutex> lock(m_mtx);
    if (m_heap.empty()) {throw std::out_of_range("Heap is empty");}
    Node root_Node = m_heap[0];
    auto root_info = std::make_pair(root_Node.GetData(), root_Node.GetRef());
    if (m_heap.size() == 1) {m_heap.pop_back();}
    if(m_heap.size() > 1){  
        m_heap[0] = m_heap.back();
        m_heap.pop_back();
        heapify_down(0);
    }
    return root_info;
} 
template <typename Traits>
istream& Heap<Traits>::read(istream& is){
        using value_type = typename Heap<Traits>::value_type;
        string line;
        if(!getline(is, line,'\0'))
            return is;
        for (char& c : line){
            if (c == '[' || c == ']' || c == '(' || c == ')' || c == ',')
                c = ' ';
        }
        value_type value;
        Ref ref;
        stringstream ss(line);
        vector<pair<value_type,Ref>> elements;
        while (ss >> value >> ref){
            elements.push_back(make_pair(value, ref));
        }
        if(!elements.empty())
            build_heap(elements);
        return is;
}
template <typename Traits>
string Heap<Traits>::to_string() const {
    std::scoped_lock<std::mutex> lock(m_mtx);
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < m_heap.size(); ++i) {
        ss << m_heap[i];
        if (i < m_heap.size() - 1) 
            ss << ", "; 
    }
     ss << "]";
    return ss.str();
}

template <typename Traits>
std::ostream& operator<<(std::ostream& os, const Heap<Traits>& heap) {
    return os << heap.to_string();
}
template <typename Traits>
istream& operator>>(istream& is, Heap<Traits>& heap){
    return heap.read(is);
}
#endif // __HEAP_H__