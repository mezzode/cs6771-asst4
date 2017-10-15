#ifndef BTREE_ITERATOR_H
#define BTREE_ITERATOR_H

#include <iterator>

/**
 * You MUST implement the btree iterators as (an) external class(es) in this file.
 * Failure to do so will result in a total mark of 0 for this deliverable.
 **/

// iterator related interface stuff here; would be nice if you called your
// iterator class btree_iterator (and possibly const_btree_iterator)

template <typename T> class btree;

template<typename T>
class BTreeIterator {
    public:
        using difference_type = ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        using btree<T>::Node;

        reference operator*() const {
            return node->elems[indices.top()];
        }
        
        pointer operator->() const {
            return &(operator*());
        }

        // prefix inc
        BTreeIterator& operator++() {
            if (node->children.size() > indices.top() && // double check this logic
                    node->children.at(indices.top()) != nullptr) {
                // if there is a child next to thingo, go to it
                node = node->children.at(indices.top()).get();
                ++indices.top(); // so in right place when return to this node
                indices.push(0);
            } else {
                ++indices.top();
                // go upwards until at valid elem
                while (indices.top() == node->elem.size()) {
                    indices.pop();
                    if (node->parent == nullptr) {
                        // at end
                        endParent = node;
                        node = nullptr;
                        break;
                    }
                    // go up
                    node = node->parent;
                }
            }
        }

        // postfix inc
        BTreeIterator operator++() {
            BTreeIterator tmp = *this;
            ++*this;
            return tmp;
        }

        // prefix dec
        BTreeIterator& operator--() {
            // TODO
        }

        // postfix dec
        BTreeIterator operator--() {
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

        BTreeIterator(Node* node_, std::stack<size_t> indices_): node{node_}, indices(indices_) { }

    private:
        Node* node;
        std::stack<size_t> indices;
        Node* endParent = nullptr;

        friend class btree<T>; // ? so can edit the iterator for find()
};

#endif
