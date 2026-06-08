#pragma once

#include "Types.hpp"
#include "BruteForce.hpp"
#include "KDTree.hpp"
#include "HNSW.hpp"
#include "Distance.hpp"

#include <unordered_map>
#include <mutex>
#include <chrono>
#include <string>

class VectorDB
{
public:

    struct Hit
    {
        int id;
        std::string metadata;
        std::string category;
        std::vector<float> emb;
        float dist;
    };

    struct SearchResult
    {
        std::vector<Hit> hits;
        long long latencyUs;
        std::string algo;
        std::string metric;
    };

    struct BenchmarkResult
    {
        long long bruteUs;
        long long kdUs;
        long long hnswUs;
        int count;
    };

private:

    std::unordered_map<int, VectorItem> store;

    BruteForce brute;
    KDTree tree;
    HNSW hnsw;

    std::mutex mu;

    int nextId;

    int dims;

public:

    explicit VectorDB(int d);

    int insert(
        const std::string& metadata,
        const std::string& category,
        const std::vector<float>& emb
    );

    bool remove(int id);

    SearchResult search(
        const std::vector<float>& query,
        int k,
        const std::string& metric,
        const std::string& algo
    );

    BenchmarkResult benchmark(
        const std::vector<float>& query,
        int k,
        const std::string& metric
    );

    std::vector<VectorItem> all();

    size_t size();

    int getDims() const;
};