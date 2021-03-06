/**
 * The btree is a linked structure which operates much like
 * a binary search tree, save the fact that multiple client
 * elements are stored in a single node.  Whereas a single element
 * would partition the tree into two ordered subtrees, a node
 * that stores m client elements partition the tree
 * into m + 1 sorted subtrees.
 */

#ifndef BTREE_H
#define BTREE_H

#include <cstddef>
#include <iostream>
#include <utility>
#include <memory>
#include <queue>

// we better include the iterator
#include "btree_iterator.h"

// we do this to avoid compiler errors about non-template friends
// what do we do, remember? :)
template <typename T> class btree;

template<typename T>
std::ostream& operator<<(std::ostream& os, const btree<T>& tree);

template <typename T>
class btree {
    public:
        /** Hmm, need some iterator typedefs here... friends? **/
        using iterator = BTreeIterator<T>;
        using const_iterator = BTreeIterator<const T>; // does this actually work?
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        friend iterator;
        friend const_iterator;

        using size_type = unsigned int;

        /**
         * Constructs an empty btree.  Note that
         * the elements stored in your btree must
         * have a well-defined zero-arg constructor,
         * copy constructor, operator=, and destructor.
         * The elements must also know how to order themselves
         * relative to each other by implementing operator<
         * and operator==. (These are already implemented on
         * behalf of all built-ins: ints, doubles, strings, etc.)
         *
         * @param maxNodeElems the maximum number of elements
         *        that can be stored in each B-Tree node
         */
        btree(size_type maxNodeElems_ = 40) : maxNodeElems{maxNodeElems_} {}

        /**
         * The copy constructor and  assignment operator.
         * They allow us to pass around B-Trees by value.
         * Although these operations are likely to be expensive
         * they make for an interesting programming exercise.
         * Implement these operations using value semantics and
         * make sure they do not leak memory.
         */

        /**
         * Copy constructor
         * Creates a new B-Tree as a copy of original.
         *
         * @param original a const lvalue reference to a B-Tree object
         */
        btree(const btree<T>& original): maxNodeElems{original.maxNodeElems} {
            // make a unique copy of what original.head points to
            head = std::make_unique<Node>(*original.head, nullptr);
        }

        /**
         * Move constructor
         * Creates a new B-Tree by "stealing" from original.
         *
         * @param original an rvalue reference to a B-Tree object
         */
        btree(btree<T>&& original): maxNodeElems{original.maxNodeElems} {
            swap(original, *this);
        }

        /**
         * Copy assignment
         * Replaces the contents of this object with a copy of rhs.
         *
         * @param rhs a const lvalue reference to a B-Tree object
         */
        btree<T>& operator=(const btree<T>& rhs) {
            // copy and swap
            btree tmp = rhs;
            swap(tmp, *this);
            return *this;
        }

        /**
         * Move assignment
         * Replaces the contents of this object with the "stolen"
         * contents of original.
         *
         * @param rhs a const reference to a B-Tree object
         */
        btree<T>& operator=(btree<T>&& rhs) {
            maxNodeElems = rhs.maxNodeElems;
            swap(rhs, *this);
            return *this;
        }

        /**
         * Puts a breadth-first traversal of the B-Tree onto the output
         * stream os. Elements must, in turn, support the output operator.
         * Elements are separated by space. Should not output any newlines.
         *
         * @param os a reference to a C++ output stream
         * @param tree a const reference to a B-Tree object
         * @return a reference to os
         */
        friend std::ostream& operator<<(std::ostream& os, const btree<T>& tree) {
            std::queue<Node*> queue;
            if (tree.head) {
                queue.push(tree.head.get());
            }
            while (!queue.empty()) {
                // add nodes' children to queue
                for (auto& child : queue.front()->children) {
                    if (child) { // child.get != nullptr
                        queue.push(child.get());
                    }
                }

                // print the nodes' contents
                const std::vector<T> elems = queue.front()->elems;
                for (size_type i = 0; i < elems.size(); ++i) {
                    os << elems.at(i); // print elem
                    if (i < elems.size() - 1 || queue.size() > 1) {
                        // only print space if something else afterward
                        // i.e. if i is not the last elem or there are more things on the queue
                        os << ' ';
                    }
                }
                queue.pop();
            }
            return os;
        }

        iterator begin() {
            return begin_();
        }

        const_iterator begin() const {
            return begin_();
        }

        const_iterator end() const {
            return end_();
        }

        iterator end() {
            return end_();
        }

        reverse_iterator rbegin() const {
            return std::reverse_iterator<iterator>(end());
        }

        reverse_iterator rend() const {
            return std::reverse_iterator<iterator>(begin());
        }

        const_iterator cbegin() const {
            return static_cast<const_iterator>(begin_());
        }

        const_iterator cend() const {
            return static_cast<const_iterator>(end_());
        }

        const_reverse_iterator crbegin() const {
            return std::reverse_iterator<const_iterator>(cend());
        }

        const_reverse_iterator crend() const {
            return std::reverse_iterator<const_iterator>(cbegin());
        }

        /**
         * Returns an iterator to the matching element, or whatever
         * the non-const end() returns if the element could
         * not be found.
         *
         * @param elem the client element we are trying to match.  The elem,
         *        if an instance of a true class, relies on the operator< and
         *        and operator== methods to compare elem to elements already
         *        in the btree.  You must ensure that your class implements
         *        these things, else code making use of btree<T>::find will
         *        not compile.
         * @return an iterator to the matching element, or whatever the
         *         non-const end() returns if no such match was ever found.
         */
        iterator find(const T& elem) {
            return find_(elem);
        }

        /**
         * Identical in functionality to the non-const version of find,
         * save the fact that what's pointed to by the returned iterator
         * is deemed as const and immutable.
         *
         * @param elem the client element we are trying to match.
         * @return an iterator to the matching element, or whatever the
         *         const end() returns if no such match was ever found.
         */
        const_iterator find(const T& elem) const {
            return static_cast<const_iterator>(find_(elem));
        }

        /**
         * Operation which inserts the specified element
         * into the btree if a matching element isn't already
         * present.  In the event where the element truly needs
         * to be inserted, the size of the btree is effectively
         * increases by one, and the pair that gets returned contains
         * an iterator to the inserted element and true in its first and
         * second fields.
         *
         * If a matching element already exists in the btree, nothing
         * is added at all, and the size of the btree stays the same.  The
         * returned pair still returns an iterator to the matching element, but
         * the second field of the returned pair will store false.  This
         * second value can be checked to after an insertion to decide whether
         * or not the btree got bigger.
         *
         * The insert method makes use of T's zero-arg constructor and
         * operator= method, and if these things aren't available,
         * then the call to btree<T>::insert will not compile.  The implementation
         * also makes use of the class's operator== and operator< as well.
         *
         * @param elem the element to be inserted.
         * @return a pair whose first field is an iterator positioned at
         *         the matching element in the btree, and whose second field
         *         stores true if and only if the element needed to be added
         *         because no matching element was there prior to the insert call.
         */
        std::pair<iterator, bool> insert(const T& elem) {
            iterator it = find(elem);
            if (it != end()) {
                // already in btree so do nothing
                return std::make_pair(it, false);
            }

            if (!head) {
                head = std::make_unique<Node>(nullptr);
            }

            Node* node = head.get();
            std::stack<size_type> indices;

            while (true) {
                // find index
                size_type i = 0;
                auto elemIt = node->elems.begin();
                auto childrenIt = node->children.begin();
                while (elemIt != node->elems.end()) {
                    if (elem < *elemIt) {
                        break;
                    }
                    ++elemIt;
                    ++i;
                    if (childrenIt != node->children.end()) {
                        ++childrenIt;
                    }
                }
                indices.push(i);

                // if node not saturated add to it and return iterator
                if (node->elems.size() < maxNodeElems) {
                    node->elems.insert(elemIt, elem);
                    if (childrenIt != node->children.end()) {
                        node->children.insert(childrenIt, nullptr);
                    }
                    return std::make_pair(iterator(node, indices), true);
                }

                // otherwise add to child
                if (i >= node->children.size()) {
                    // extend to fit
                    node->children.resize(i + 1);
                }
                if (node->children[i] == nullptr) {
                    // create actual child if doesnt exist yet
                    node->children[i] = std::make_unique<Node>(node);
                }
                node = node->children[i].get();
            }
        }

        /**
         * Disposes of all internal resources, which includes
         * the disposal of any client objects previously
         * inserted using the insert operation.
         * Check that your implementation does not leak memory!
         */
        ~btree() = default;

    private:
        struct Node {
            Node(Node* parent_): parent{parent_} {};

            Node(const Node& original, Node* parent_): elems(original.elems), parent{parent_} {
                for (const auto& child : original.children) {
                    if (child != nullptr) {
                        // make a unique copy of each child
                        children.push_back(std::make_unique<Node>(*child, this));
                    } else {
                        children.push_back(nullptr);
                    }
                }
            }

            std::vector<T> elems;
            std::vector<std::unique_ptr<Node>> children;
            Node* parent;
        };

        std::unique_ptr<Node> head;
        size_type maxNodeElems;

        iterator begin_() const {
            if (!head) {
                return iterator(std::stack<size_type>(), nullptr);
            }

            Node* node = head.get();
            std::stack<size_type> indices;
            while (!node->children.empty() && node->children.at(0)) { // node->children.at(0).get() != nullptr
                node = node->children.at(0).get();
                indices.push(0);
            }
            indices.push(0);
            return iterator(node, indices);
        }

        iterator end_() const {
            if (!head) {
                return iterator(std::stack<size_type>(), nullptr);
            }

            Node* node = head.get();
            std::stack<size_type> indices;

            // get the size of elems; if there is a child at that index (i.e. something larger than the largest elem), go to that. otherwise take the largest elem.
            while (!node->children.empty() && node->elems.size() == node->children.size() - 1) {
                indices.push(node->elems.size());
                node = node->children.at(node->elems.size()).get();
            }
            indices.push(node->elems.size() - 1);
            // does not add extra index to indices
            return iterator(indices, node);
        }

        iterator find_(const T& elem) const {
            Node* node = head.get();
            std::stack<size_type> indices;
            size_type i = 0;
            while (node != nullptr && i <= node->elems.size()) {
                if (i == node->elems.size() || elem < node->elems.at(i) ) { // if at end of elems look at final child or if elem is smaller then should look at child to left
                    // look in that child
                    if (i >= node->children.size()) {
                        // no child
                        return end_();
                    }
                    node = node->children.at(i).get();
                    indices.push(i);
                    i = 0;
                } else if (node->elems.at(i) == elem) {
                    // found
                    indices.push(i);
                    return iterator(node, indices);
                } else {
                    ++i;
                }
            }
            return end_();
        }

        friend void swap(btree<T>& a, btree<T>& b) {
            using std::swap;
            swap(a.head, b.head);
            swap(a.maxNodeElems, b.maxNodeElems);
        }
};

#endif
