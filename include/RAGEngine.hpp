#pragma once

#include "DocumentDB.hpp"
#include "OllamaClient.hpp"
#include <string>
#include <vector>

class RAGEngine
{
private:

    DocumentDB db;

    OllamaClient ollama;

public:

    RAGEngine();

    void addDocument(
        const std::string& title,
        const std::string& text
    );

    std::vector<DocItem>
    retrieve(
        const std::string& question,
        int k = 3
    );

    std::string ask(
        const std::string& question,
        int k = 3
    );

    std::vector<DocItem> allDocuments();

    size_t documentCount();
};