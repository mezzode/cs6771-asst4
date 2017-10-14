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

// we better include the iterator
#include "btree_iterator.h"

// we do this to avoid compiler errors about non-template friends
// what do we do, remember? :)

template <typename T>
class btree {
    public:
        /** Hmm, need some iterator typedefs here... friends? **/
        using iterator = BTreeIterator<T*>;
        using const_iterator = BTreeIterator<const T*>; // does this actually work?
        using reverse_iterator = decltype(std::make_reverse_iterator(begin()));
        using const_reverse_iterator = decltype(std::make_reverse_iterator(cbegin()));
        friend class BTreeIterator<T>; // or <T*>?

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
        btree(size_t maxNodeElems = 40);

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
        btree(const btree<T>& original);

        /**
         * Move constructor
         * Creates a new B-Tree by "stealing" from original.
         *
         * @param original an rvalue reference to a B-Tree object
         */
        btree(btree<T>&& original);

        /**
         * Copy assignment
         * Replaces the contents of this object with a copy of rhs.
         *
         * @param rhs a const lvalue reference to a B-Tree object
         */
        btree<T>& operator=(const btree<T>& rhs);

        /**
         * Move assignment
         * Replaces the contents of this object with the "stolen"
         * contents of original.
         *
         * @param rhs a const reference to a B-Tree object
         */
        btree<T>& operator=(btree<T>&& rhs);

        /**
         * Puts a breadth-first traversal of the B-Tree onto the output
         * stream os. Elements must, in turn, support the output operator.
         * Elements are separated by space. Should not output any newlines.
         *
         * @param os a reference to a C++ output stream
         * @param tree a const reference to a B-Tree object
         * @return a reference to os
         */
        friend std::ostream& operator<<<T>(std::ostream& os, const btree<T>& tree) {
            std::queue<btree<T>> queue = {tree};
            while (!queue.empty()) {
                // add nodes' children to queue
                for (const btree<T>& child : queue.front().children) {
                    if (child != nullptr) {
                        queue.push(child);
                    }
                }

                // print the nodes' contents
                const std::vector<T> elems = queue.front().elems;
                for (size_t i = 0; i < elems.size(); ++i) {
                    os << elems.at(i); // print elem
                    if (i < elems.size() - 1 || queue.size() > 1) {
                        // only print space if something else afterward
                        // i.e. if i is not the last elem or there are more things on the queue
                        // (but what if the other children are empty? but shouldn't be possible for them to be empty?)
                        os << ' ';
                    }
                }
                queue.pop();
            }
            return os;
        }

        /**
         * The following can go here
         * -- begin()
         * -- end()
         * -- rbegin()
         * -- rend()
         * -- cbegin()
         * -- cend()
         * -- crbegin()
         * -- crend()
         */
        iterator begin();
        iterator end();
        reverse_iterator rbegin() {
            return std::make_reverse_iterator(end());
        }
        reverse_iterator rend() {
            return std::make_reverse_iterator(begin());
        }
        const_iterator cbegin();
        const_iterator cend();
        const_reverse_iterator crbegin() {
            return std::make_reverse_iterator(cend());
        }
        const_reverse_iterator crend() {
            return std::make_reverse_iterator(cbegin());
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
            std::stack<std::weak_ptr<btree>> levels = {std::make_shared<btree>()}; // not sure if this will work
            for (size_t i = 0; i < elems.size(); ++i) {
                if (elems.at(i) == elem) {
                    // found

                    // TODO: construct and return iterator

                }
            }
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
        const_iterator find(const T& elem) const;

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
        std::pair<iterator, bool> insert(const T& elem);

        /**
         * Disposes of all internal resources, which includes
         * the disposal of any client objects previously
         * inserted using the insert operation.
         * Check that your implementation does not leak memory!
         */
        ~btree();

    private:
        btree(size_t maxNodeElems = 40) : maxNodeElems_{maxNodeElems} {}

        struct Node {
            std::vector<T> elems;
            std::vector<std::shared_ptr<btree>> children;
            std::weak_ptr<btree> parent; // or raw poiner?

            std::pair<iterator, bool> insert(const T& elem) {
                if (elems.size() < maxNodeElems_) {
                    // node not saturated so add to it
                    for (auto it = elems.begin(); it != elems.end(); ++it) {
                        if (elem < *it) {
                            break;
                        }
                    }
                    elems.insert(it, elem);  // insert elem before iterator
                    // TODO: return iterator and true
                } else {
                    // node saturated to add to child
                    // loop through elems to find which child to add to
                    int childx = 0;
                    for (auto it = elems.begin(); it != elems.end(); ++it, ++childx) {
                        if (elem < *it) {
                            // adding to the child before this elem (and after the prev elem)
                            break;
                        }
                    }
                    if (childx >= children.size()) {
                        // extend to fit
                        children.resize(childx + 1);
                    }
                    if (children[childx] == nullptr) {
                        // create actual child if doesnt exist yet
                        children[childx] = std::make_unique<btree>(maxNodeElems_);
                    }
                    return children[childx].insert(elem);

                    // if storing parents in stack, need to pass in an iterator and edit it while we recurse deeper
                    // would still need to do similar for indices regardless of in stack or in node...
                    // maybe dont use recursion, just a loop
                }
            }
        }

        std::shared_ptr<btree> head;

        std::pair<iterator, bool> insert(const T& elem) {
            iterator it = find(elem);
            if (it != end()) {
                // already in btree so do nothing
                return std::make_pair(it, false);
            }

            std::shared_ptr<btree> node = head; // or raw?
            std::stack<size_t> indices;

            while (true) {
                // find index
                auto& elems = levels.top().elems;
                int i = 0;
                for (auto it = elems.begin(); it != elems.end(); ++it, ++i) {
                    if (elem < *it) {
                        break;
                    }
                }
                indices.push(i);

                // if node not saturated add to it and return iterator
                if (elems.size() < maxNodeElems_) {
                    elems.insert(it, elem);
                    iterator insertedIt;
                    insertedIt.node = node;
                    insertedIt.indices = indices;
                    return std::make_pair(insertedIt, true);
                }

                // otherwise add to child
                if (i >= node->children.size()) {
                    // extend to fit
                    node->children.resize(i + 1);
                }
                if (node->children[i] == nullptr) {
                    // create actual child if doesnt exist yet
                    node->children[i] = std::make_shared<btree>(maxNodeElems_);
                }
                node = node->children[i];
            }
        }
};

#endif
