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
                Node<std::remove_const_t<T>>* originalNode = node;
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
            // if (node->children.size() < indices.top()-1 && // double check this logic
            //         node->children.at(indices.top()-1) != nullptr) {
            //     // if there is a child next to thingo, go to it
            //     node = node->children.at(indices.top()-1).get();
            //     indices.push(node->elems.size() - 1); // now that i think of it, need to recurse down? i.e. prev elem will be rightmost thing to its left.
            // } else {
            //     Node<std::remove_const_t<T>>* originalNode = node;
            //     auto originalIndices = indices;
            //     --indices.top();
            //     // go upwards until at valid elem
            //     while (indices.top() == node->elem.size()) {
            //         indices.pop();
            //         if (node->parent == nullptr) {
            //             // at end
            //             endParent = originalNode;
            //             node = nullptr;
            //             indices = originalIndices;
            //             indices.push(0);
            //             break;
            //         }
            //         // go up
            //         node = node->parent;
            //     }
            // }
            // return *this;
            // if (indices.top() - 1) // the child with the same index as an elem is the left child
        }

        // postfix dec
        BTreeIterator operator--(int) {
            BTreeIterator tmp = *this;
            --*this;
            return tmp;
        }

        bool operator==(const BTreeIterator& other) const {
            return node == other.node && indices == other.indices;
        }

        bool operator!=(const BTreeIterator& other) const {
            return !operator==(other);
        } 

        // casting iterators to const_iterators
        operator BTreeIterator<const T>() {
            // return BTreeIterator<const T>(static_cast<Node<const T>*>(node), indices, static_cast<Node<const T>*>(endParent));
            return BTreeIterator<const T>(node, indices, endParent);
        }

        BTreeIterator(Node<std::remove_const_t<T>>* node_, std::stack<size_type> indices_): BTreeIterator(node_, indices_, nullptr) { }
        
        BTreeIterator(std::stack<size_type> indices_, Node<std::remove_const_t<T>>* endParent_): node{nullptr}, indices{indices_}, endParent{endParent_} { }

        BTreeIterator(Node<std::remove_const_t<T>>* node_, std::stack<size_type> indices_, Node<std::remove_const_t<T>>* endParent_): node{node_}, indices{indices_}, endParent{endParent_} { }

        // BTreeIterator(const Node<std::remove_const_t<T>>* node_, std::stack<size_type> indices_, const Node<std::remove_const_t<T>>* endParent_): node{node_}, indices{indices_}, endParent{endParent_} { }

    private:
        Node<std::remove_const_t<T>>* node;
        std::stack<size_type> indices;
        Node<std::remove_const_t<T>>* endParent;
};

#endif
