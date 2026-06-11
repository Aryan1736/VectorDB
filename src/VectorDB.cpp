#include "VectorDB.hpp"

VectorDB::VectorDB(int d)
    :
    tree(d),
    hnsw(getDistFn("cosine")),
    nextId(1),
    dims(d)
{
}

int VectorDB::insert(
    const std::string& metadata,
    const std::string& category,
    const std::vector<float>& emb
)
{
    std::lock_guard<std::mutex> lk(mu);

    if((int)emb.size() != dims)
        return -1;

    VectorItem v;

    v.id = nextId++;
    v.metadata = metadata;
    v.category = category;
    v.emb = emb;

    store[v.id] = v;

    brute.insert(v);

    tree.insert(v);

    hnsw.insert(v);

    return v.id;
}

bool VectorDB::remove(int id)
{
    std::lock_guard<std::mutex> lk(mu);

    if(!store.count(id))
        return false;

    store.erase(id);

    brute.remove(id);

    hnsw.remove(id);

    std::vector<VectorItem> remaining;

    for(auto& [key, value] : store)
    {
        remaining.push_back(value);
    }

    tree.rebuild(remaining);

    return true;
}

VectorDB::SearchResult
VectorDB::search(
    const std::vector<float>& query,
    int k,
    const std::string& metric,
    const std::string& algo
)
{
    std::lock_guard<std::mutex> lk(mu);

    if(k <= 0 || (int)query.size() != dims)
        return {{}, 0, algo, metric};

    auto distFn =
        getDistFn(metric);

    auto start =
        std::chrono::high_resolution_clock::now();

    std::vector<std::pair<float,int>> raw;

    if(algo == "bruteforce")
    {
        raw =
            brute.knn(
                query,
                k,
                distFn
            );
    }
    else if(algo == "kdtree")
    {
        raw =
            tree.knn(
                query,
                k,
                distFn
            );
    }
    else
    {
        raw =
            hnsw.knn(
                query,
                k,
                50
            );
    }

    auto end =
        std::chrono::high_resolution_clock::now();

    SearchResult result;

    result.latencyUs =
        std::chrono::duration_cast<
            std::chrono::microseconds
        >(end-start).count();

    result.algo = algo;
    result.metric = metric;

    for(auto& [dist,id] : raw)
    {
        if(!store.count(id))
            continue;

        auto& v =
            store[id];

        result.hits.push_back(
        {
            v.id,
            v.metadata,
            v.category,
            v.emb,
            dist
        });
    }

    return result;
}

VectorDB::BenchmarkResult
VectorDB::benchmark(
    const std::vector<float>& query,
    int k,
    const std::string& metric
)
{
    std::lock_guard<std::mutex> lk(mu);

    if(k <= 0 || (int)query.size() != dims)
        return {0, 0, 0, (int)store.size()};

    auto distFn =
        getDistFn(metric);

    auto timeFn =
        [&](auto fn)
        {
            auto start =
                std::chrono::high_resolution_clock::now();

            fn();

            auto end =
                std::chrono::high_resolution_clock::now();

            return std::chrono::duration_cast
                <
                    std::chrono::microseconds
                >
                (end - start)
                .count();
        };

    long long bruteUs =
        timeFn(
            [&]()
            {
                brute.knn(
                    query,
                    k,
                    distFn
                );
            }
        );

    long long kdUs =
        timeFn(
            [&]()
            {
                tree.knn(
                    query,
                    k,
                    distFn
                );
            }
        );

    long long hnswUs =
        timeFn(
            [&]()
            {
                hnsw.knn(
                    query,
                    k,
                    50
                );
            }
        );

    return
    {
        bruteUs,
        kdUs,
        hnswUs,
        (int)store.size()
    };
}

HNSW::GraphInfo
VectorDB::hnswInfo()
{
    std::lock_guard<std::mutex> lk(mu);

    return hnsw.getInfo();
}

std::vector<VectorItem>
VectorDB::all()
{
    std::lock_guard<std::mutex> lk(mu);

    std::vector<VectorItem> result;

    for(auto& [id, item] : store)
    {
        result.push_back(item);
    }

    return result;
}

size_t VectorDB::size()
{
    std::lock_guard<std::mutex> lk(mu);

    return store.size();
}

int VectorDB::getDims() const
{
    return dims;
}
