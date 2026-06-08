#pragma once

#include "Types.hpp"
#include "BruteForce.hpp"
#include "HNSW.hpp"

#include <unordered_map>
#include <mutex>
#include <string>

struct DocItem
{
    int id;

    std::string title;

    std::string text;

    std::vector<float> emb;
};

class DocumentDB
{
private:

    std::unordered_map<int, DocItem> store;

    HNSW hnsw;

    BruteForce brute;

    std::mutex mu;

    int nextId;

    int dims;

public:

    DocumentDB();

    int insert(
        const std::string& title,
        const std::string& text,
        const std::vector<float>& emb
    );

    std::vector<
        std::pair<float, DocItem>
    >
    search(
        const std::vector<float>& query,
        int k,
        float maxDist = 0.7f
    );

    bool remove(int id);

    std::vector<DocItem> all();

    size_t size();

    int getDims() const;
};