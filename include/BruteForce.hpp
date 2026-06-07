#pragma once

#include "Types.hpp"
#include "Distance.hpp"

#include <vector>
#include <utility>

class BruteForce {
private:
    std::vector<VectorItem> items;

public:
    void insert(const VectorItem& v);

    std::vector<std::pair<float,int>>
    knn(
        const std::vector<float>& q,
        int k,
        DistFn dist
    );

    void remove(int id);
};