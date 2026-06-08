#include <iostream>

#include "Distance.hpp"
#include "BruteForce.hpp"
#include "KDTree.hpp"

int main()
{
    BruteForce brute;
    KDTree tree(3);

    VectorItem v1{
        1,
        "item1",
        "demo",
        {1,2,3}
    };

    VectorItem v2{
        2,
        "item2",
        "demo",
        {10,10,10}
    };

    VectorItem v3{
        3,
        "item3",
        "demo",
        {2,3,4}
    };

    brute.insert(v1);
    brute.insert(v2);
    brute.insert(v3);

    tree.insert(v1);
    tree.insert(v2);
    tree.insert(v3);

    auto bruteAns =
        brute.knn(
            {1.5,2.5,3.5},
            2,
            getDistFn("euclidean")
        );

    auto treeAns =
        tree.knn(
            {1.5,2.5,3.5},
            2,
            getDistFn("euclidean")
        );

    std::cout << "\n=== Brute Force ===\n";

    for (auto& p : bruteAns)
    {
        std::cout
            << "id = "
            << p.second
            << " dist = "
            << p.first
            << '\n';
    }

    std::cout << "\n=== KD Tree ===\n";

    for (auto& p : treeAns)
    {
        std::cout
            << "id = "
            << p.second
            << " dist = "
            << p.first
            << '\n';
    }

    return 0;
}