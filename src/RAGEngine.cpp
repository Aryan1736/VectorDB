#include "RAGEngine.hpp"

#include "Chunker.hpp"

#include <sstream>

RAGEngine::RAGEngine()
{
}

void RAGEngine::addDocument(
    const std::string& title,
    const std::string& text
)
{
    auto chunks =
        Chunker::chunkText(
            text,
            200,
            30
        );

    for(size_t i = 0; i < chunks.size(); i++)
    {
        auto emb =
            ollama.embed(
                chunks[i]
            );

        if(emb.empty())
            continue;

        std::string chunkTitle =
            title +
            " [chunk " +
            std::to_string(i + 1) +
            "]";

        db.insert(
            chunkTitle,
            chunks[i],
            emb
        );
    }
}

std::vector<DocItem>
RAGEngine::retrieve(
    const std::string& question,
    int k
)
{
    auto qEmb =
        ollama.embed(
            question
        );

    auto results =
        db.search(
            qEmb,
            k
        );

    std::vector<DocItem> docs;

    for(auto& [dist, doc] : results)
    {
        docs.push_back(doc);
    }

    return docs;
}

std::string
RAGEngine::ask(
    const std::string& question,
    int k
)
{
    auto docs =
        retrieve(
            question,
            k
        );

    std::stringstream prompt;

    prompt
        << "Answer the question using the provided context.\n\n";

    prompt
        << "Context:\n";

    for(auto& doc : docs)
    {
        prompt
            << doc.text
            << "\n\n";
    }

    prompt
        << "Question:\n"
        << question
        << "\n\nAnswer:";

    return ollama.generate(
        prompt.str()
    );
}

std::vector<DocItem>
RAGEngine::allDocuments()
{
    return db.all();
}

size_t
RAGEngine::documentCount()
{
    return db.size();
}