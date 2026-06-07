#include <iostream>
#include "Distance.hpp"
#include "BruteForce.hpp"

int main()
{
    BruteForce db;

    db.insert({
        1,
        "item1",
        "demo",
        {1,2,3}
    });

    db.insert({
        2,
        "item2",
        "demo",
        {10,10,10}
    });

    db.insert({
        3,
        "item3",
        "demo",
        {2,3,4}
    });

    auto ans =
        db.knn(
            {1.5,2.5,3.5},
            2,
            getDistFn("euclidean")
        );

    for(auto& p : ans)
    {
        std::cout
            << "id = "
            << p.second
            << " dist = "
            << p.first
            << '\n';
    }
}