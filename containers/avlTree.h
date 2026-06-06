#ifndef __AVL_TREE_H__
#define __AVL_TREE_H__
#include "binarytree.h"

template <typename Traits>
class AVLTree : public BinaryTree<Traits> {
public:
    using value_type    = typename Traits::value_type;
    using Node          = typename BinaryTree<Traits>::Node;
    using Parent        = typename BinaryTree<Traits>::MySelf;

    Node* insert(const value_type &value, Ref ref) {
        scoped_lock<recursive_mutex> lock(this->m_mutex);
        Node* insertedNode = Parent::insert(value, ref);
        if(insertedNode==this->getRoot()) return insertedNode;
        balanceAfterInsert(insertedNode);
        return insertedNode;
    }
private:
    void balanceAfterInsert(Node* current) {
        while (current) {
            TI oldHeight = current->getHeight();
            updateHeight(current);
            TI balance = getBalance(current);
            if(balance > 1 || balance < -1){
                size_t dir = balance > 1 ? 0 : 1;
                current = anyRotation(current, dir, balance);
                break;
            }
            if (current->getHeight() == oldHeight) break;
            current = current->getParent();
        }
    }
    TI getHeightSafe(Node* node) { return node ? node->getHeight() : 0;}
    void updateHeight(Node* node) {
        if (!node) return;
        TI lHeight = getHeightSafe(node->getChild(0));
        TI rHeight = getHeightSafe(node->getChild(1));
        node->setHeight(1 + max(lHeight, rHeight));
    }
    TI getBalance(Node* node) {
        return node ? getHeightSafe(node->getChild(0)) - getHeightSafe(node->getChild(1)): 0;
    }
    Node* anyRotation (Node* current, size_t dir, TI balance) {
        TI childBalance = getBalance(current->getChild(dir));
        if(childBalance * balance < 0)
            rotate(current->getChild(dir), 1 - dir); 
        return rotate(current, dir);
    }
    // dir = 0 (giro a la derecha, para balancear un subárbol izquierdo)
    // dir = 1 (giro a la izquierda, para balancear un subárbol derecho)
    Node* rotate(Node* node, size_t dir) {
        size_t oppDir = 1 - dir;
        Node* x = node->getChild(dir); 
        Node* parent = node->getParent();
        node->setChild(dir, x->getChild(oppDir));
        x->setChild(oppDir, node);
        updateHeight(node);
        updateHeight(x);
        if(!parent){
            this->m_pRoot = x;
            x->setParent(nullptr);
            return x;
        }
        parent->setChild(parent->getChild(1) == node, x);
        return x;
    }
public:
    istream& read_binaryTree(std::istream& is) {
        std::scoped_lock<std::recursive_mutex> lock(this->m_mutex);
        Parent::read_binaryTree(is);
        rebuildToAVL();
        return is;
    }
private:
    void rebuildToAVL() {
        //falta implementar
    }
};
#endif // __AVL_TREE_H__