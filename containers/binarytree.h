#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__
#include <iostream>
#include <cstddef>   // size_t
#include <string>
#include <utility> 
#include <map>
#include <vector> 
#include <sstream>
#include <mutex>
#include "general_iterator.h"
#include "basetrait.h"
#include "../foreach.h"
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
        if (current->getChild(Direction::a)) 
            return current->getChild(Direction::a);
        if (current->getChild(Direction::b)) 
            return current->getChild(Direction::b);
        auto parent = current->getParent();
        while (parent && 
                (current == parent->getChild(Direction::b) || parent->getChild(Direction::b) == nullptr)) {
            current = parent;
            parent = parent->getParent();
        }
        return (parent) ? parent->getChild(Direction::b) : nullptr;
        
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
    using NodePtr = typename Container::Node*;
public:
    static NodePtr getBeginNode(NodePtr root){
        return order::template begin<NodePtr, directionWalk>(root);
    }
    MySelf& operator++(){
        this->m_pNode = order::template next<NodePtr, directionWalk>(this->m_pNode);
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
using AscendingBinaryTreeTrait = customTrait<T, AscendingTrait<T>>;
template <typename T>
using DescendingBinaryTreeTrait = customTrait<T, DescendingTrait<T>>;

template <typename Traits>
class BinaryTree{
public:    
    using value_type = typename Traits::value_type;
    using Comp       = typename Traits::Comp;
    using MySelf     = BinaryTree<Traits>;

    using forward_inorder_iterator  = BinaryTreeForwardInorderIterator<MySelf>;
    using backward_inorder_iterator = BinaryTreeBackwardInorderIterator<MySelf>;

    using forward_preorder_iterator = BinaryTreeForwardPreorderIterator<MySelf>;
    using backward_preorder_iterator = BinaryTreeBackwardPreorderIterator<MySelf>;
    
    using forward_postorder_iterator = BinaryTreeForwardPostorderIterator<MySelf>;
    using backward_postorder_iterator = BinaryTreeBackwardPostorderIterator<MySelf>;

    class Node{ 
    protected:
        value_type  m_data;
        Ref         m_ref;
        TI          m_height = 1;
        Node*       m_pChild[2] = {nullptr, nullptr};
        Node*       m_pParent= nullptr;   
    public:
        Node(const value_type& data, const Ref& ref, 
            Node* left = nullptr, Node* right = nullptr)
            : m_data(data), m_ref(ref)
        {
            setChild(0, left);
            setChild(1, right);
        }
        Node(const Node& other) 
            : m_data(other.m_data), m_ref(other.m_ref), m_pParent(nullptr) 
        {
            m_pChild[0] = nullptr;
            m_pChild[1] = nullptr;
        }
        Node(Node&& other) noexcept:
            m_data   (move(other.m_data)),
            m_ref    (move(other.m_ref)),
            m_pParent(nullptr)
        {
            m_pChild[0] = exchange(other.m_pChild[0], nullptr);
            m_pChild[1] = exchange(other.m_pChild[1], nullptr);
            if(m_pChild[0]) m_pChild[0]->m_pParent = this;
            if(m_pChild[1]) m_pChild[1]->m_pParent = this;
        }
        ~Node() {};

        value_type      getData() const { return m_data; }
        value_type&     getDataRef()    { return m_data; }
        void            setData(value_type data) { m_data = data; }

        Ref             getRef() const  { return m_ref; }
        Ref&            getRefRef()     { return m_ref; }
        void            setRef(Ref ref) { m_ref = ref; }

        TI              getHeight() const { return m_height; }
        void            setHeight(TI height) { m_height = height; }

        Node*           getParent() const { return m_pParent;}
        void            setParent(Node* pParent) { m_pParent = pParent; }

        Node*           getChild(size_t pos) const { return m_pChild[pos]; }
        Node*&          getChildRef(size_t pos)    { return m_pChild[pos]; }
        
        void setChild(size_t pos, Node* pChild) { 
            m_pChild[pos] = pChild; 
            if (pChild) 
                pChild->m_pParent = this;
        }
        friend ostream& operator<<(ostream& os, const Node& node) {
            os << "{" << node.m_data << ", " << node.m_ref << "}";
            return os;
        }
    };

protected:
    Node*   m_pRoot = nullptr;
    Comp    m_comp;
    size_t  m_size = 0;
    mutable recursive_mutex m_mutex;
    
    struct BinaryRecord {
        value_type  m_data;
        Ref         m_ref;
        TI          m_childId[2]; 
    };
public:
    BinaryTree(): m_pRoot(nullptr), m_size(0) {}
    virtual ~BinaryTree() { clear(); }
    void clear() { 
        scoped_lock<recursive_mutex> lock(m_mutex);
        if(!m_pRoot) return;
        vector<Node*> stack;
        stack.push_back(m_pRoot);
        while(!stack.empty()){
            Node* current = stack.back();
            stack.pop_back();
            if(current->getChild(0)){ 
                current->getChild(0)->setParent(nullptr);
                stack.push_back(current->getChild(0));
            }
            if(current->getChild(1)){ 
                current->getChild(1)->setParent(nullptr);
                stack.push_back(current->getChild(1));
            }
            delete current;
        }
        m_pRoot = nullptr;
        m_size = 0;
    }
    BinaryTree(const BinaryTree &other){ // Copy constructor
        scoped_lock<recursive_mutex> lock(other.m_mutex); 
        m_pRoot = clone_BinaryTree(other.m_pRoot);
        m_size  = other.m_size; 
    }
    BinaryTree(BinaryTree &&other) noexcept { // Move constructor
        scoped_lock<recursive_mutex> lock(other.m_mutex); 
        m_pRoot = exchange(other.m_pRoot, nullptr);
        m_size  = exchange(other.m_size, 0);
    }
    size_t size() const { 
        scoped_lock<recursive_mutex> lock(m_mutex); 
        return m_size;
    }
    Node* getRoot() const { 
        scoped_lock<recursive_mutex> lock(m_mutex); 
        return m_pRoot; 
    }
    Node* insert(const value_type &value, Ref ref){
        scoped_lock<recursive_mutex> lock(m_mutex);
        if (!m_pRoot) {
            m_pRoot = new Node(value, ref);
            m_size++;
            return m_pRoot;
        }
        Node* current = m_pRoot;
        while (true) {
            size_t pos = !m_comp(value, current->getDataRef());
            if (!current->getChild(pos)) {
                current->setChild(pos, new Node(value, ref));
                m_size++;
                return current->getChild(pos);
            }
            current = current->getChild(pos);
        }
    }
    ostream& write_BinaryTree(std::ostream& os) const {
        std::scoped_lock<std::recursive_mutex> lock(m_mutex);
        size_t totalNodes = m_size;
        os.write(reinterpret_cast<const char*>(&totalNodes), sizeof(totalNodes));
        if (totalNodes == 0) return os;
        std::map<Node*, TI> nodeToId; 
        std::vector<Node*> indexToNode;
        TI currentId = 0;       
        std::vector<Node*> stack;
        stack.push_back(m_pRoot);
        while (!stack.empty()) {
            Node* current = stack.back();
            stack.pop_back();
            nodeToId[current] = currentId++;
            indexToNode.push_back(current);
            if (current->getChild(1)) stack.push_back(current->getChild(1));
            if (current->getChild(0)) stack.push_back(current->getChild(0));
        }
        
        for (Node* pNode : indexToNode) {
            BinaryRecord record;
            record.m_data = pNode->getData();
            record.m_ref  = pNode->getRef();
            for (size_t i = 0; i < 2; ++i) {
                record.m_childId[i] = pNode->getChild(i) ? nodeToId[pNode->getChild(i)] : -1;
            }
            os.write(reinterpret_cast<const char*>(&record), sizeof(BinaryRecord));
        }
        return os;
    }

    istream& read_binaryTree(std::istream& is) {
        clear(); 
        std::scoped_lock<std::recursive_mutex> lock(m_mutex);
        size_t totalNodes = 0;
        if (!is.read(reinterpret_cast<char*>(&totalNodes), sizeof(totalNodes))) 
            return is; 
        if (totalNodes == 0) return is;
        vector<Node*> idToNode(totalNodes, nullptr);
        vector<BinaryRecord> records(totalNodes);
        for (size_t i = 0; i < totalNodes; ++i) {
            is.read(reinterpret_cast<char*>(&records[i]), sizeof(BinaryRecord));
            idToNode[i] = new Node(records[i].m_data, records[i].m_ref);
            if (i == 0) {
                m_pRoot = idToNode[i];
            }
        }
        m_size = totalNodes;
        for (size_t i = 0; i < totalNodes; ++i) {
            Node* pNode = idToNode[i];
            const BinaryRecord& rec = records[i];
            for (size_t j = 0; j < 2; ++j) {
                if (rec.m_childId[j] != -1) 
                    pNode->setChild(j, idToNode[rec.m_childId[j]]);
            }
        }
        return is;
    }
private:
    Node* clone_BinaryTree(Node* source_root) {
        if (!source_root) return nullptr;
        Node* dest_root = new Node(*source_root);
        vector<pair<Node*, Node*>> stack;
        stack.push_back({source_root, dest_root});

        while (!stack.empty()) {
            auto [other_node, my_node] = stack.back();
            stack.pop_back();
            if (other_node->getChild(0)) {
                my_node->setChild(0, new Node(*other_node->getChild(0)));
                stack.push_back({other_node->getChild(0), my_node->getChild(0)});
            }
            if (other_node->getChild(1)) {
                my_node->setChild(1, new Node(*other_node->getChild(1)));
                stack.push_back({other_node->getChild(1), my_node->getChild(1)});
            }
        }
        return dest_root;
    }
    template <typename Iterator>
    Iterator begin() {
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

    template <typename Iterator>
    struct rangeView {
        Iterator m_begin;
        Iterator m_end;
        unique_lock<recursive_mutex> m_mutex;
        rangeView(Iterator begin, Iterator end, recursive_mutex& mutex) 
            : m_begin(begin), m_end(end), m_mutex(mutex) {}
        Iterator begin(){ return m_begin; }
        Iterator end()  { return m_end; }

        template <typename Func, typename... Args>
        void ForEach(Func func, Args &&...  args){
            ::ForEach(m_begin, m_end, func, std::forward<Args>(args)... );
        }
        template <typename Func, typename... Args>
        auto FirstThat(Func func, Args &&...  args){
            return ::FirstThat(m_begin, m_end, func, std::forward<Args>(args)... );
        }
    };

    auto forward_inorder(){return rangeView<forward_inorder_iterator>(begin_forward_inorder(), end_forward_inorder(), m_mutex);}
    auto backward_inorder(){return rangeView<backward_inorder_iterator>(begin_backward_inorder(), end_backward_inorder(), m_mutex);}
    auto forward_preorder(){return rangeView<forward_preorder_iterator>(begin_forward_preorder(), end_forward_preorder(), m_mutex);}
    auto backward_preorder(){return rangeView<backward_preorder_iterator>(begin_backward_preorder(), end_backward_preorder(), m_mutex);}
    auto forward_postorder(){return rangeView<forward_postorder_iterator>(begin_forward_postorder(), end_forward_postorder(), m_mutex);}
    auto backward_postorder(){return rangeView<backward_postorder_iterator>(begin_backward_postorder(), end_backward_postorder(), m_mutex);}
};
template <typename Traits>
ostream& operator<<(ostream& os, const BinaryTree<Traits>& tree) {
        return tree.write_BinaryTree(os);
}

template <typename Traits>
istream& operator>>(istream& is, BinaryTree<Traits>& tree) {
        return tree.read_binaryTree(is);
}



#endif // __BINARY_TREE_H__