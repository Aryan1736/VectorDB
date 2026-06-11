#pragma once

#include "VectorDB.hpp"
#include "DocumentDB.hpp"
#include "OllamaClient.hpp"
#include "RAGEngine.hpp"

class Server
{
private:

    VectorDB db;
    RAGEngine rag;
    OllamaClient ollama;

public:

    Server();

    void start(
        int port = 8080
    );
};