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

        reference operator*() const {
            std::shared_ptr<btree> node = levels.top().lock();
            return node->elems[indices.top()]; // how return reference
        }
        
        pointer operator->() const {
            return &(operator*());
        }

        // prefix inc
        BTreeIterator& operator++() {
            std::shared_ptr<btree> node = levels.top().lock();
            if (node->children.size() > indices.top() && // double check this logic
                    node->children.at(indices.top()) != nullptr) {
                // if there is a child next to thingo, go to it
                levels.push(children.at(indices.top()));
                indices.push(0);
            } else {
                ++indices.top();
                if (indices.top() == node->children.size()) {
                    // no more in this node, go to parent
                    levels.pop();
                    indices.pop();
                    ++indices.top();
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
            return levels == other.levels && indices == other.indices;
        }

        bool operator!=(const BTreeIterator& other) const {
            return !operator==(other);
        } 

        BTreeIterator() {
            
        }

    private:
        std::stack<std::weak_ptr<btree>> levels; // weak or shared?
        std::stack<size_t> indices;

        friend class btree<T>; // ? so can edit the iterator for find()
};

#endif
