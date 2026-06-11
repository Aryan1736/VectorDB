#pragma once

#include "VectorDB.hpp"
#include "DocumentDB.hpp"
#include "OllamaClient.hpp"

class Server
{
private:

    VectorDB db;
    DocumentDB docDB;
    OllamaClient ollama;

public:

    Server();

    void start(
        int port = 8080
    );
};