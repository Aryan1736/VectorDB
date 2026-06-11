#include "DocumentDB.hpp"

DocumentDB::DocumentDB()
    :
    hnsw(getDistFn("cosine")),
    nextId(1)
{
}

int DocumentDB::insert(
    const std::string& title,
    const std::string& text,
    const std::vector<float>& emb
)
{
    std::lock_guard<std::mutex> lk(mu);

    DocItem doc;

    doc.id = nextId++;

    doc.title = title;

    doc.text = text;

    doc.emb = emb;

    store[doc.id] = doc;

    VectorItem vi;

    vi.id = doc.id;
    vi.metadata = title;
    vi.category = "document";
    vi.emb = emb;

    brute.insert(vi);

    hnsw.insert(vi);

    return doc.id;
}

bool DocumentDB::remove(int id)
{
    std::lock_guard<std::mutex> lk(mu);

    if(!store.count(id))
        return false;

    store.erase(id);

    hnsw.remove(id);

    brute.remove(id);

    return true;
}

std::vector<
    std::pair<float, DocItem>
>
DocumentDB::search(
    const std::vector<float>& query,
    int k,
    float maxDist
)
{
    std::lock_guard<std::mutex> lk(mu);

    auto raw =
        hnsw.knn(
            query,
            k,
            50
        );

    std::vector<
        std::pair<float, DocItem>
    > result;

    for(auto& [dist,id] : raw)
    {
        if(!store.count(id))
            continue;

        result.push_back(
        {
            dist,
            store[id]
        });
    }

    return result;
}

std::vector<DocItem>
DocumentDB::all()
{
    std::lock_guard<std::mutex> lk(mu);

    std::vector<DocItem> result;

    for(auto& [id,doc] : store)
    {
        result.push_back(doc);
    }

    return result;
}

size_t DocumentDB::size()
{
    std::lock_guard<std::mutex> lk(mu);

    return store.size();
}