#ifndef BTREE_ITERATOR_H
#define BTREE_ITERATOR_H

#include <iterator>
#include <stack>

/**
 * You MUST implement the btree iterators as (an) external class(es) in this file.
 * Failure to do so will result in a total mark of 0 for this deliverable.
 **/

// iterator related interface stuff here; would be nice if you called your
// iterator class btree_iterator (and possibly const_btree_iterator)

template <typename T> class btree;
template <typename T> class Node;

template<typename T>
class BTreeIterator {
    public:
        using difference_type = ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        // using size_type = typename btree<T>::size_type;
        using size_type = unsigned int;
        using Node = typename btree<std::remove_const_t<T>>::Node;

        reference operator*() const {
            return node->elems[indices.top()];
        }

        pointer operator->() const {
            return &(operator*());
        }

        // prefix inc
        BTreeIterator& operator++() {
            // go to leftmost thing that is to the right of the current node
            // if there is a child to the right (i.e. in index i+1), take the leftmost thing in that subtree
            // else take the elem to the right (i.e. i+1)
            // if no elem or child to the right, take the elem to the right in the parent
            // if no elem to the right in the parent go to the next parent. continue as necessary
            if (indices.top() + 1 < node->children.size() && // double check this logic
                    node->children.at(indices.top() + 1)) {
                // if there is a child to the right, go to it
                ++indices.top();
                node = node->children.at(indices.top()).get();
                indices.push(0);
                // go to smallest elem in the right subtree
                while (!node->children.empty() && node->children.at(0)) {
                    node = node->children.at(0).get();
                    indices.push(0);
                }
            } else {
                Node* originalNode = node;
                auto originalIndices = indices;
                ++indices.top();
                // go upwards until at valid elem
                while (indices.top() == node->elems.size()) {
                    indices.pop();
                    if (node->parent == nullptr) {
                        // at end
                        endParent = originalNode;
                        node = nullptr;
                        indices = originalIndices;
                        // indices.push(0); // should end() have a trailing 0 index on the stack? ensure consistent with --
                        break;
                    }
                    // go up
                    node = node->parent;
                }
            }
            return *this;
        }

        // postfix inc
        BTreeIterator operator++(int) {
            BTreeIterator tmp = *this;
            ++*this;
            return tmp;
        }

        // prefix dec
        BTreeIterator& operator--() {
            if (node == nullptr && endParent != nullptr) {
                // at end
                node = endParent;
                endParent = nullptr;
            } else if (indices.top() < node->children.size() && // double check this logic
                    node->children.at(indices.top())) {
                // if there is a child to the left, go to it
                // ++indices.top();
                node = node->children.at(indices.top()).get();
                indices.push(node->elems.size() - 1); // be careful of index
                // go to largest elem in the left subtree
                while (node->children.size() == node->elems.size() + 1 && node->children.at(node->elems.size())) {
                    // while there is a child to the right of the final elem
                    node = node->children.at(node->elems.size()).get();
                    indices.push(node->elems.size() - 1);
                }
            } else {
                // go upwards until at valid elem
                while (indices.top() == 0) {
                    indices.pop(); // going to parent from the left child, so need to decrement index
                    // but if index is 0, need to go from parent to child again
                    // go up
                    node = node->parent;
                }
                --indices.top();
            }
            return *this;
        }

        // postfix dec
        BTreeIterator operator--(int) {
            BTreeIterator tmp = *this;
            --*this;
            return tmp;
        }

        bool operator==(const BTreeIterator& other) const {
            return node == other.node && indices == other.indices && endParent == other.endParent;
        }

        bool operator!=(const BTreeIterator& other) const {
            return !operator==(other);
        }

        // casting iterators to const_iterators
        operator BTreeIterator<const T>() const {
            // return BTreeIterator<const T>(static_cast<Node<const T>*>(node), indices, static_cast<Node<const T>*>(endParent));
            return BTreeIterator<const T>(node, indices, endParent);
        }

        BTreeIterator(Node* node_, std::stack<size_type> indices_): BTreeIterator(node_, indices_, nullptr) { }

        BTreeIterator(std::stack<size_type> indices_, Node* endParent_): node{nullptr}, indices{indices_}, endParent{endParent_} { }

        BTreeIterator(Node* node_, std::stack<size_type> indices_, Node* endParent_): node{node_}, indices{indices_}, endParent{endParent_} { }

        // BTreeIterator(const Node* node_, std::stack<size_type> indices_, const Node* endParent_): node{node_}, indices{indices_}, endParent{endParent_} { }

    private:
        Node* node;
        std::stack<size_type> indices;
        Node* endParent;
};

template <typename T>
bool operator==(const BTreeIterator<T>& a, const BTreeIterator<const T>& b) {
    return static_cast<BTreeIterator<const T>>(a) == b;
}

template <typename T>
bool operator!=(const BTreeIterator<T>& a, const BTreeIterator<const T>& b) {
    return !(a == b);
}

#endif
