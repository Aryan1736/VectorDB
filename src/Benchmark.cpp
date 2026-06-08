#include "Benchmark.hpp"

#include "BruteForce.hpp"
#include "KDTree.hpp"
#include "HNSW.hpp"
#include "Distance.hpp"

#include <chrono>
#include <iostream>
#include <random>

void runBenchmark()
{
    const int N = 10000;

    BruteForce brute;
    const int DIM = 128;

    KDTree tree(DIM);

    HNSW hnsw(
        getDistFn("euclidean")
    );

    std::mt19937 rng(42);

    std::uniform_real_distribution<float>
        rd(0.0f,100.0f);

    for(int i=0;i<N;i++)
    {
        VectorItem v;

        v.id = i;

        for(int j = 0; j < DIM; j++)
        {
            v.emb.push_back(
                rd(rng)
            );
        }

        brute.insert(v);
        tree.insert(v);
        hnsw.insert(v);
    }

    std::vector<float> query;

    for(int i = 0; i < DIM; i++)
    {
        query.push_back(
            rd(rng)
        );
    }

    auto start =
        std::chrono::high_resolution_clock::now();

    auto bruteAns =
        brute.knn(
            query,
            10,
            getDistFn("euclidean")
        );

    auto end =
        std::chrono::high_resolution_clock::now();

    auto bruteTime =
        std::chrono::duration_cast<
            std::chrono::microseconds
        >(end-start);

    start =
        std::chrono::high_resolution_clock::now();

    auto treeAns =
        tree.knn(
            query,
            10,
            getDistFn("euclidean")
        );

    end =
        std::chrono::high_resolution_clock::now();

    auto treeTime =
        std::chrono::duration_cast<
            std::chrono::microseconds
        >(end-start);

    start =
        std::chrono::high_resolution_clock::now();

    auto hnswAns =
        hnsw.knn(
            query,
            10,
            50
        );

    end =
        std::chrono::high_resolution_clock::now();

    auto hnswTime =
        std::chrono::duration_cast<
            std::chrono::microseconds
        >(end-start);

    std::cout << "\n===== BENCHMARK =====\n";

    std::cout
        << "Vectors: "
        << N
        << "\n\n";

    std::cout
        << "Brute Force : "
        << bruteTime.count()
        << " us\n";

    std::cout
        << "KD Tree     : "
        << treeTime.count()
        << " us\n";

    std::cout
        << "HNSW        : "
        << hnswTime.count()
        << " us\n";
}