#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__
#include <iostream>
#include <cstddef>   // size_t
#include <string>
#include <utility>  
#include <sstream>
#include <mutex>
#include "general_iterator.h"
#include "basetrait.h"
#include "../types.h"
using namespace std;
struct forwardWalk{
    static constexpr size_t a = 0;
    static constexpr size_t b = 1;
};
struct backwardWalk{
    static constexpr size_t a = 1;
    static constexpr size_t b = 0;
};
// Tipo de recorrido: InOrder, PreOrder, PostOrder
struct InOrder {
    template <typename NodePtr, typename Direction>
    static NodePtr begin(NodePtr root) {
        if (!root) return nullptr;
        auto current = root;
        while (current->getChild(Direction::a)) {
            current = current->getChild(Direction::a);
        }
        return current;
    }

    template <typename NodePtr, typename Direction>
    static NodePtr next(NodePtr current) {
        if (!current) return nullptr;

        if (current->getChild(Direction::b)) {
            current = current->getChild(Direction::b);
            while (current->getChild(Direction::a)) {
                current = current->getChild(Direction::a);
            }
            return current;
        } else {
            auto parent = current->getParent();
            while (parent != nullptr && current == parent->getChild(Direction::b)) {
                current = parent;            
                parent = parent->getParent(); 
            }
            return parent;
        }
    }
};
struct PreOrder {
    template <typename NodePtr, typename Direction>
    static NodePtr begin(NodePtr root) {
        return root;
    }

    template <typename NodePtr, typename Direction>
    static NodePtr next(NodePtr current) {
        if (!current) return nullptr;

        if (current->getChild(Direction::a)) {
            return current->getChild(Direction::a);
        } else if (current->getChild(Direction::b)) {
            return current->getChild(Direction::b);
        } else {
            auto parent = current->getParent();
            while (parent != nullptr && 
                  (current == parent->getChild(Direction::b) || parent->getChild(Direction::b) == nullptr)) {
                current = parent;
                parent = parent->getParent();
            }
            return (parent != nullptr) ? parent->getChild(Direction::b) : nullptr;
        }
    }
};
struct PostOrder {
    template <typename NodePtr, typename Direction>
    static NodePtr begin(NodePtr root) {
        if (!root) return nullptr;
        auto current = root;
        while (true) {
            if (current->getChild(Direction::a)) {
                current = current->getChild(Direction::a);
            } else if (current->getChild(Direction::b)) {
                current = current->getChild(Direction::b);
            } else {
                break;
            }
        }
        return current;
    }

    template <typename NodePtr, typename Direction>
    static NodePtr next(NodePtr current) {
        if (!current) return nullptr;
        auto parent = current->getParent();
        if (!parent) return nullptr; 
        if (current == parent->getChild(Direction::b)) return parent;
        if (parent->getChild(Direction::b)) 
            return begin<NodePtr, Direction>(parent->getChild(Direction::b));
        return parent;
    }
};

template <typename Container, typename order, typename directionWalk>
class BinaryTreeWalkIterator :   public general_iterator<Container, 
                                        BinaryTreeWalkIterator<Container, order, directionWalk>>,
                                        public directionWalk
{
    using MySelf = BinaryTreeWalkIterator<Container, order, directionWalk>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    static typename Container::NodePtr getBeginNode(typename Container::NodePtr root){
        return order::template begin<typename Container::NodePtr, directionWalk>(root);
    }
    MySelf& operator++(){
        this->m_pNode = order::template next<typename Container::NodePtr, directionWalk>(this->m_pNode);
        return *this;
    }
};

template <typename Container>
using BinaryTreeForwardInorderIterator = BinaryTreeWalkIterator<Container, InOrder, forwardWalk>;
template <typename Container>
using BinaryTreeBackwardInorderIterator = BinaryTreeWalkIterator<Container, InOrder, backwardWalk>;

template <typename Container>
using BinaryTreeForwardPreorderIterator = BinaryTreeWalkIterator<Container, PreOrder, forwardWalk>;
template <typename Container>
using BinaryTreeBackwardPreorderIterator = BinaryTreeWalkIterator<Container, PostOrder, backwardWalk>;

template <typename Container>
using BinaryTreeForwardPostorderIterator = BinaryTreeWalkIterator<Container, PostOrder, forwardWalk>;
template <typename Container>
using BinaryTreeBackwardPostorderIterator = BinaryTreeWalkIterator<Container, PreOrder, backwardWalk>;

template <typename T>
class BinaryTreeNode{
public:
    using value_type = T;
    using Node       = BinaryTreeNode<T>;
    using NodePtr    = Node*;
protected:
    value_type m_data;
    Ref        m_ref;
    NodePtr    m_pChild[2] = {nullptr, nullptr};
    NodePtr    m_pParent;   
public:
    BinaryTreeNode(const value_type& data, const Ref& ref, 
        NodePtr left = nullptr, NodePtr right = nullptr)
        : m_data(data), m_ref(ref)
    {
        setChild(0, left);
        setChild(1, right);
    }
    // copy constructor ... revisar
    BinaryTreeNode(const BinaryTreeNode& other){ // Evitar deadlock
        m_data = other.m_data;
        m_ref  = other.m_ref;   
        m_pParent = nullptr; 
        if (other.m_pChild[0]){ 
            m_pChild[0] = new Node(*other.m_pChild[0]);
            m_pChild[0]->m_pParent = this; 
        }
        if (other.m_pChild[1]){
            m_pChild[1] = new Node(*other.m_pChild[1]);
            m_pChild[1]->m_pParent = this;
        }
    }
    // Corregir con exchange
    BinaryTreeNode(BinaryTreeNode&& other) noexcept:
        m_data   (move(other.m_data)),
        m_ref    (move(other.m_ref)),
        m_pParent(nullptr)
    {
        m_pChild[0] = exchange(other.m_pChild[0], nullptr);
        m_pChild[1] = exchange(other.m_pChild[1], nullptr);
        if(m_pChild[0]) m_pChild[0]->m_pParent = this;
        if(m_pChild[1]) m_pChild[1]->m_pParent = this;
    }
    ~BinaryTreeNode() {
        delete m_pChild[0];
        delete m_pChild[1];
    };

    value_type      getData() const { return m_data; }
    value_type&     getDataRef()    { return m_data; }
    void            setData(value_type data) { m_data = data; }

    Ref             getRef() const  { return m_ref; }
    Ref&            getRefRef()     { return m_ref; }
    void            setRef(Ref ref) { m_ref = ref; }

    NodePtr         getParent() const { return m_pParent;}

    NodePtr         getChild(size_t pos) const { return m_pChild[pos]; }
    NodePtr&        getChildRef(size_t pos)    { return m_pChild[pos]; }
    
    void setChild(size_t pos, NodePtr pChild) { 
        m_pChild[pos] = pChild; 
        if (pChild) 
            pChild->m_pParent = this;
    }

    string to_string() const {
        stringstream ss;
        ss << "Node(data: " << m_data << ", ref: " << m_ref << ")";
        return ss.str();
    }
    // Cuidado: en el disco hay posiciones dentro del archivo,
    //          en memoria hay punteros
    friend ostream& operator<<(ostream& os, const BinaryTreeNode& node) {
        return os << node.to_string(); 
    }

    // Cuidado: en el disco hay posiciones dentro del archivo,
    //          en memoria hay punteros
    friend istream& operator>>(istream& is, BinaryTreeNode& node) {
        string line;
        if (getline(is, line)) {
            stringstream ss(line);
            ss >> node.m_data >> node.m_ref;
        }
        return is;
    }
};
template <typename T>
using AscendingBinaryTreeTrait = customTrait<T, BinaryTreeNode<T>, AscendingTrait<T>>;
template <typename T>
using DescendingBinaryTreeTrait = customTrait<T, BinaryTreeNode<T>, DescendingTrait<T>>;

template <typename Traits>
class BinaryTree{
public:
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using NodePtr   = Node*;
    using Comp       = typename Traits::Comp;
    using MySelf     = BinaryTree<Traits>;

    using forward_inorder_iterator  = BinaryTreeForwardInorderIterator<MySelf>;
    using backward_inorder_iterator = BinaryTreeBackwardInorderIterator<MySelf>;

    using forward_preorder_iterator = BinaryTreeForwardPreorderIterator<MySelf>;
    using backward_preorder_iterator = BinaryTreeBackwardPreorderIterator<MySelf>;
    
    using forward_postorder_iterator = BinaryTreeForwardPostorderIterator<MySelf>;
    using backward_postorder_iterator = BinaryTreeBackwardPostorderIterator<MySelf>;
protected:
    Node* m_pRoot = nullptr;
    Comp m_comp;
    size_t m_size = 0;
    mutable mutex m_mutex; 
public:
    BinaryTree(): m_pRoot(nullptr), m_size(0) {}
    ~BinaryTree() { clear(); }
    void clear() { 
        scoped_lock<mutex> lock(m_mutex);
        delete m_pRoot; 
        m_pRoot = nullptr; 
        m_size = 0; }
    BinaryTree(const BinaryTree &other){ // Copy constructor
        scoped_lock<mutex> lock(other.m_mutex); 
        if (other.m_pRoot) m_pRoot = new Node(*other.m_pRoot); 
    };
    BinaryTree(BinaryTree &&other){ // Move constructor
        scoped_lock<mutex> lock(other.m_mutex); 
        m_pRoot = exchange(other.m_pRoot, nullptr);
        m_size  = exchange(other.m_size, 0);
    };
    size_t size() const { scoped_lock<mutex> lock(m_mutex); return m_size;}
    void insert(const value_type &value, Ref ref){
        scoped_lock<mutex> lock(m_mutex);
        if (!m_pRoot) {
            m_pRoot = new Node(value, ref);
            m_size++;
            return;
        }
        internal_insert(m_pRoot, value, ref);
    }
private:
    void internal_insert(Node* &pNode, const value_type &value, Ref ref){
        size_t pos = !m_comp(value, pNode->getDataRef());
        if(!pNode->getChildRef(pos)){
            pNode->setChild(pos, new Node(value, ref));
            m_size++;
            return;
        }else{
            internal_insert(pNode->getChildRef(pos), value, ref);
        }
    }
    template <typename Iterator>
    Iterator begin() {
        //scoped_lock<mutex> lock(m_mutex);
        return Iterator(this, Iterator::getBeginNode(m_pRoot));
    }
    template <typename Iterator>
    Iterator end() {
        return Iterator(this, nullptr);
    }
public:
    auto begin_forward_inorder()    { return begin<forward_inorder_iterator>();}
    auto end_forward_inorder()      { return end<forward_inorder_iterator>(); }
    auto begin_backward_inorder()   { return begin<backward_inorder_iterator>(); }
    auto end_backward_inorder()     { return end<backward_inorder_iterator>(); }

    auto begin_forward_preorder()   { return begin<forward_preorder_iterator>(); }
    auto end_forward_preorder()     { return end<forward_preorder_iterator>(); }
    auto begin_backward_preorder()  { return begin<backward_preorder_iterator>(); }
    auto end_backward_preorder()    { return end<backward_preorder_iterator>(); }
    
    auto begin_forward_postorder()  { return begin<forward_postorder_iterator>(); }
    auto end_forward_postorder()    { return end<forward_postorder_iterator>(); }
    auto begin_backward_postorder() { return begin<backward_postorder_iterator>(); }
    auto end_backward_postorder()   { return end<backward_postorder_iterator>(); }
};




#endif // __BINARY_TREE_H__