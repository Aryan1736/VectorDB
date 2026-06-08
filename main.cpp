#include <iostream>

#include "Chunker.hpp"
#include "DocumentDB.hpp"
#include "Embedder.hpp"

int main()
{
    std::string text =
        "Deadlock occurs when processes "
        "wait forever for resources.";

    auto emb =
        Embedder::embed(text);

    std::cout
        << "Embedding dims: "
        << emb.size()
        << "\n";

    DocumentDB db;

    db.insert(
        "OS Notes",
        text,
        emb
    );

    auto result =
        db.search(
            emb,
            1
        );

    std::cout
        << "Found: "
        << result[0].second.title
        << "\n";

    return 0;
}