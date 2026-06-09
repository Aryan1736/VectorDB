#include <iostream>

#include "RAGEngine.hpp"

int main()
{
    RAGEngine rag;

    rag.addDocument(
        "OS Notes",
        "Deadlock occurs when processes wait forever "
        "for resources. CPU scheduling determines "
        "which process gets the CPU."
    );

    std::cout
        << "Documents: "
        << rag.documentCount()
        << "\n\n";

    std::string answer =
        rag.ask(
            "What is deadlock?"
        );

    std::cout
        << answer
        << "\n";
}