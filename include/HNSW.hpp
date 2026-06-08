#pragma once

#include "Types.hpp"
#include "Distance.hpp"

#include <unordered_map>
#include <vector>
#include <queue>
#include <random>
#include <string>

class HNSW
{
private:

    struct Node
    {
        VectorItem item;
        int maxLyr;
        std::vector<std::vector<int>> nbrs;
    };

    std::unordered_map<int, Node> G;

    int M;
    int M0;
    int ef_build;

    float mL;

    int topLayer = -1;
    int entryPt  = -1;

    DistFn distFn;

    std::mt19937 rng;

    int randLevel();

    std::vector<std::pair<float,int>>
    searchLayer(
        const std::vector<float>& q,
        int ep,
        int ef,
        int lyr
    );

    std::vector<int>
    selectNbrs(
        std::vector<std::pair<float,int>>& cands,
        int maxM
    );

public:

    explicit HNSW(
        DistFn dist,
        int m = 16,
        int efBuild = 200
    );

    void insert(const VectorItem& item);

    std::vector<std::pair<float,int>>
    knn(
        const std::vector<float>& q,
        int k,
        int ef
    );

    void remove(int id);

    size_t size() const;
};