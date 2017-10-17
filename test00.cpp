#include <vector>

#include "btree.h"

int main(void) {
    std::vector<int> v = {3, 5, 1, 4, 6, 2};
    btree<int> tree = btree<int>(2);

    for (auto n : v) {
        auto result = tree.insert(n);
        if (result.second) {
            std::cout << "Inserted " << n << "\n";
        }
    }

    std::cout << tree << "\n";

    for (auto n : v) {
        auto result = tree.find(n);
        std::cout << (result == tree.end() ? "Didn't find " : "Found ") << n << "\n";
    }

    const auto tree2 = tree;
    std::cout << tree2 << "\n";

    for (auto n : v) {
        auto result = tree2.find(n);
        std::cout << (result == tree2.end() ? "Didn't find " : "Found ") << n << "\n";
    }

    for (auto it = tree.cbegin(); it != tree.cend(); ++it) {
        std::cout << *it << "\n";
    }

    for (auto it = tree.crbegin(); it != tree.crend(); ++it) {
        std::cout << *it << "\n";
    }

    std::cout.setf(std::ios::boolalpha);

    std::cout << (tree.cend() == tree.end()) << "\n"; // is only allowing conversion one way?
    std::cout << (tree.end() == tree.cend()) << "\n"; // is only allowing conversion one way?
    std::cout << (tree2.cend() == tree2.end()) << "\n";
    std::cout << (tree2.end() == tree2.cend()) << "\n";

    std::cout << (tree.cend() != tree.end()) << "\n"; // is only allowing conversion one way?
    std::cout << (tree.end() != tree.cend()) << "\n"; // is only allowing conversion one way?
    std::cout << (tree2.cend() != tree2.end()) << "\n";
    std::cout << (tree2.end() != tree2.cend()) << "\n";

    std::cout << (tree.end() == tree2.end()) << "\n";
    std::cout << (tree.cend() == tree2.cend()) << "\n";
    std::cout << (tree2.end() == tree.end()) << "\n";
    std::cout << (tree2.cend() == tree.cend()) << "\n";

    std::cout << (tree.end() != tree2.end()) << "\n";
    std::cout << (tree.cend() != tree2.cend()) << "\n";
    std::cout << (tree2.end() != tree.end()) << "\n";
    std::cout << (tree2.cend() != tree.cend()) << "\n";

    auto a = std::move(tree);

    std::cout << tree << "\n";
    std::cout << a << "\n";

    btree<int> b;
    b = std::move(a);
    std::cout << a << "\n";
    std::cout << b << "\n";
}