#include "Server.hpp"
#include "DemoData.hpp"
#include "third_party/httplib.h"
#include "JsonUtils.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

static void cors(
    httplib::Response& res
)
{
    res.set_header(
        "Access-Control-Allow-Origin",
        "*"
    );

    res.set_header(
        "Access-Control-Allow-Methods",
        "GET, POST, DELETE, OPTIONS"
    );

    res.set_header(
        "Access-Control-Allow-Headers", 
        "Content-Type"
    );
}

static std::string jsonEscape(
    const std::string& s
)
{
    std::string out;

    for(char c : s)
    {
        if(c == '"')
            out += "\\\"";
        else if(c == '\\')
            out += "\\\\";
        else if(c == '\n')
            out += "\\n";
        else if(c == '\r')
            out += "\\r";
        else if(c == '\t')
            out += "\\t";
        else
            out += c;
    }

    return out;
}

static std::string vecJson(
    const std::vector<float>& v
)
{
    std::ostringstream ss;

    ss << "[";

    for(size_t i = 0; i < v.size(); i++)
    {
        if(i)
            ss << ",";

        ss << v[i];
    }

    ss << "]";

    return ss.str();
}

static std::vector<float> extractVec(
    const std::string& body,
    const std::string& key
)
{
    size_t p =
        body.find(
            "\"" + key + "\""
        );

    if(p == std::string::npos)
        return {};

    p = body.find('[', p);

    if(p == std::string::npos)
        return {};

    size_t e =
        body.find(']', p);

    if(e == std::string::npos)
        return {};

    return parseVec(
        body.substr(
            p + 1,
            e - p - 1
        )
    );
}

static int extractInt(
    const std::string& body,
    const std::string& key,
    int fallback
)
{
    size_t p =
        body.find(
            "\"" + key + "\""
        );

    if(p == std::string::npos)
        return fallback;

    p = body.find(':', p);

    if(p == std::string::npos)
        return fallback;

    try
    {
        return std::stoi(
            body.substr(p + 1)
        );
    }
    catch(...)
    {
        return fallback;
    }
}

static std::string preview(
    const std::string& s,
    size_t maxLen = 140
)
{
    if(s.size() <= maxLen)
        return s;

    return s.substr(0, maxLen - 3) + "...";
}

static int wordCount(
    const std::string& s
)
{
    std::istringstream ss(s);
    std::string word;
    int count = 0;

    while(ss >> word)
        count++;

    return count;
}

Server::Server()
    :
    db(16)
{
    loadDemoData(db);
}

void Server::start(
    int port
)
{
    httplib::Server svr;

    svr.Options(
        ".*",
        [](
            const httplib::Request&,
            httplib::Response& res
        )
        {
            cors(res);
            res.status = 204;
        }
    );

    svr.Get(
        "/",
        [&](const httplib::Request&,
            httplib::Response& res)
        {
            cors(res);

            std::ifstream file(
                "index.html"
            );

            if(!file)
            {
                res.status = 404;

                res.set_content(
                    "index.html not found",
                    "text/plain"
                );

                return;
            }

            std::stringstream buffer;

            buffer << file.rdbuf();

            res.set_content(
                buffer.str(),
                "text/html"
            );
        }
    );

    // ==========================
    // GET /status
    // ==========================

    svr.Get(
        "/status",
        [&](const httplib::Request&,
            httplib::Response& res)
        {
            cors(res);

            bool online =
                ollama.isAvailable();

            std::ostringstream ss;

            ss
                << "{"
                << "\"ollama\":"
                << (online ? "\"online\"" : "\"offline\"")
                << ","
                << "\"ollamaAvailable\":"
                << (online ? "true" : "false")
                << ","
                << "\"embedModel\":\"nomic-embed-text\""
                << ","
                << "\"genModel\":\"llama3.2\""
                << ","
                << "\"docDims\":"
                << rag.documentDims()
                << ","
                << "\"docCount\":"
                << rag.documentCount()
                << "}";

            res.set_content(
                ss.str(),
                "application/json"
            );
        }
    );

    // ==========================
    // GET /stats
    // ==========================

    svr.Get(
        "/stats",
        [&](const httplib::Request&,
            httplib::Response& res)
        {
            cors(res);

            std::ostringstream ss;

            ss
                << "{"
                << "\"vectors\":"
                << db.size()
                << ","
                << "\"documents\":"
                << rag.documentCount()
                << "}";

            res.set_content(
                ss.str(),
                "application/json"
            );
        }
    );

    // ==========================
    // GET /items
    // ==========================

    svr.Get(
        "/items",
        [&](const httplib::Request&,
            httplib::Response& res)
        {
            cors(res);

            auto items =
                db.all();

            std::ostringstream ss;

            ss << "[";

            for(size_t i = 0; i < items.size(); i++)
            {
                if(i)
                    ss << ",";

                ss
                    << "{"
                    << "\"id\":"
                    << items[i].id
                    << ","
                    << "\"metadata\":\""
                    << jsonEscape(items[i].metadata)
                    << "\","
                    << "\"category\":\""
                    << jsonEscape(items[i].category)
                    << "\","
                    << "\"embedding\":"
                    << vecJson(items[i].emb)
                    << "}";
            }

            ss << "]";

            res.set_content(
                ss.str(),
                "application/json"
            );
        }
    );

    // ==========================
    // POST /insert
    // ==========================

    svr.Post(
        "/insert",
        [&](const httplib::Request& req,
            httplib::Response& res)
        {
            cors(res);

            std::string metadata =
                extractStr(
                    req.body,
                    "metadata"
                );

            std::string category =
                extractStr(
                    req.body,
                    "category"
                );

            auto emb =
                extractVec(
                    req.body,
                    "embedding"
                );

            if(
                metadata.empty()
                ||
                category.empty()
                ||
                (int)emb.size() != db.getDims()
            )
            {
                res.status = 400;

                res.set_content(
                    "{\"error\":\"metadata, category, and 16D embedding are required\"}",
                    "application/json"
                );

                return;
            }

            int id =
                db.insert(
                    metadata,
                    category,
                    emb
                );

            std::ostringstream ss;

            ss
                << "{"
                << "\"status\":\"ok\","
                << "\"id\":"
                << id
                << "}";

            res.set_content(
                ss.str(),
                "application/json"
            );
        }
    );

    // ==========================
    // DELETE /delete/:id
    // ==========================

    svr.Delete(
        R"(/delete/(\d+))",
        [&](const httplib::Request& req,
            httplib::Response& res)
        {
            cors(res);

            int id =
                std::stoi(
                    req.matches[1]
                );

            bool ok =
                db.remove(id);

            if(!ok)
                res.status = 404;

            res.set_content(
                ok
                ? "{\"status\":\"ok\"}"
                : "{\"error\":\"not found\"}",
                "application/json"
            );
        }
    );

    // ==========================
    // GET /search
    // ==========================

    svr.Get(
        "/search",
        [&](const httplib::Request& req,
            httplib::Response& res)
        {
            cors(res);

            if(!req.has_param("v"))
            {
                res.status = 400;

                res.set_content(
                    "{\"error\":\"missing vector\"}",
                    "application/json"
                );

                return;
            }

            std::string vecStr =
                req.get_param_value("v");

            int k = 5;

            if(req.has_param("k"))
            {
                k =
                    std::stoi(
                        req.get_param_value("k")
                    );
            }

            std::string metric =
                "cosine";

            if(req.has_param("metric"))
            {
                metric =
                    req.get_param_value("metric");
            }

            std::string algo =
                "hnsw";

            if(req.has_param("algo"))
            {
                algo =
                    req.get_param_value("algo");
            }

            auto vec =
                parseVec(
                    vecStr
                );

            if((int)vec.size() != db.getDims())
            {
                res.status = 400;

                res.set_content(
                    "{\"error\":\"query vector must have 16 dimensions\"}",
                    "application/json"
                );

                return;
            }

            auto out =
                db.search(
                    vec,
                    k,
                    metric,
                    algo
                );

            std::ostringstream ss;

            ss << "{";

            ss
                << "\"latency_us\":"
                << out.latencyUs
                << ",";

            ss
                << "\"latencyUs\":"
                << out.latencyUs
                << ",";

            ss
                << "\"algo\":\""
                << out.algo
                << "\",";

            ss
                << "\"metric\":\""
                << out.metric
                << "\",";

            ss << "\"hits\":[";

            for(size_t i = 0; i < out.hits.size(); i++)
            {
                if(i)
                    ss << ",";

                auto& h =
                    out.hits[i];

                ss
                    << "{"
                    << "\"id\":"
                    << h.id
                    << ","
                    << "\"metadata\":\""
                    << jsonEscape(h.metadata)
                    << "\","
                    << "\"category\":\""
                    << jsonEscape(h.category)
                    << "\","
                    << "\"distance\":"
                    << h.dist
                    << "}";
            }

            ss << "],";

            ss << "\"results\":[";

            for(size_t i = 0; i < out.hits.size(); i++)
            {
                if(i)
                    ss << ",";

                auto& h =
                    out.hits[i];

                ss
                    << "{"
                    << "\"id\":"
                    << h.id
                    << ","
                    << "\"metadata\":\""
                    << jsonEscape(h.metadata)
                    << "\","
                    << "\"category\":\""
                    << jsonEscape(h.category)
                    << "\","
                    << "\"distance\":"
                    << h.dist
                    << "}";
            }

            ss << "]}";

            res.set_content(
                ss.str(),
                "application/json"
            );
        }
    );

    // ==========================
    // GET /benchmark
    // ==========================
    svr.Get(
        "/benchmark",
        [&](const httplib::Request& req,
            httplib::Response& res)
        {
            cors(res);

            if(!req.has_param("v"))
            {
                res.status = 400;

                res.set_content(
                    "{\"error\":\"missing vector\"}",
                    "application/json"
                );

                return;
            }

            auto query =
                parseVec(
                    req.get_param_value("v")
                );

            if((int)query.size() != db.getDims())
            {
                res.status = 400;

                res.set_content(
                    "{\"error\":\"query vector must have 16 dimensions\"}",
                    "application/json"
                );

                return;
            }

            int k = 5;

            if(req.has_param("k"))
            {
                k =
                    std::stoi(
                        req.get_param_value("k")
                    );
            }

            std::string metric =
                "cosine";

            if(req.has_param("metric"))
            {
                metric =
                    req.get_param_value("metric");
            }

            auto out =
                db.benchmark(
                    query,
                    k,
                    metric
                );

            std::ostringstream ss;

            ss
                << "{"
                << "\"items\":"
                << out.count
                << ","
                << "\"bruteforce_us\":"
                << out.bruteUs
                << ","
                << "\"bruteforceUs\":"
                << out.bruteUs
                << ","
                << "\"kdtree_us\":"
                << out.kdUs
                << ","
                << "\"kdtreeUs\":"
                << out.kdUs
                << ","
                << "\"hnsw_us\":"
                << out.hnswUs
                << ","
                << "\"hnswUs\":"
                << out.hnswUs
                << "}";

            res.set_content(
                ss.str(),
                "application/json"
            );
        }
    );

    // ==========================
    // GET /HNSW INFO
    // ==========================
    svr.Get(
        "/hnsw-info",
        [&](const httplib::Request&,
            httplib::Response& res)
        {
            cors(res);

            auto info =
                db.hnswInfo();

            std::ostringstream ss;

            ss
                << "{"
                << "\"topLayer\":"
                << info.topLayer
                << ","
                << "\"nodeCount\":"
                << info.nodeCount
                << ","
                << "\"nodesPerLayer\":[";

            for(size_t i = 0; i < info.nodesPerLayer.size(); i++)
            {
                if(i)
                    ss << ",";

                ss << info.nodesPerLayer[i];
            }

            ss << "],\"edgesPerLayer\":[";

            for(size_t i = 0; i < info.edgesPerLayer.size(); i++)
            {
                if(i)
                    ss << ",";

                ss << info.edgesPerLayer[i];
            }

            ss << "]}";

            res.set_content(
                ss.str(),
                "application/json"
            );
        }
    );

    // ==========================
    // POST /doc/insert
    // ==========================
    svr.Post(
        "/doc/insert",
        [&](const httplib::Request& req,
            httplib::Response& res)
        {
            std::cout
                << "BODY:\n"
                << req.body
                << "\n";

            cors(res);

            std::string title =
                extractStr(
                    req.body,
                    "title"
                );

            std::string text =
                extractStr(
                    req.body,
                    "text"
                );

            if(
                title.empty()
                ||
                text.empty()
            )
            {
                res.status = 400;

                res.set_content(
                    "{\"error\":\"missing fields\"}",
                    "application/json"
                );

                return;
            }

            int chunks =
                rag.addDocument(
                title,
                text
            );

            if(chunks <= 0)
            {
                res.status = 503;

                res.set_content(
                    "{\"error\":\"no embeddings created; is Ollama running?\"}",
                    "application/json"
                );

                return;
            }

            std::ostringstream ss;

            ss
                << "{"
                << "\"status\":\"ok\","
                << "\"chunks\":"
                << chunks
                << ","
                << "\"dims\":"
                << rag.documentDims()
                << "}";

            res.set_content(
                ss.str(),
                "application/json"
            );
        }
    );

    // ==========================
    // GET /doc/list
    // ==========================
    svr.Get(
        "/doc/list",
        [&](const httplib::Request&,
            httplib::Response& res)
        {
            cors(res);

            auto docs =
                rag.allDocuments();

            std::ostringstream ss;

            ss << "[";

            for(size_t i = 0; i < docs.size(); i++)
            {
                if(i)
                    ss << ",";

                ss
                    << "{"
                    << "\"id\":"
                    << docs[i].id
                    << ","
                    << "\"title\":\""
                    << jsonEscape(docs[i].title)
                    << "\","
                    << "\"preview\":\""
                    << jsonEscape(preview(docs[i].text))
                    << "\","
                    << "\"words\":"
                    << wordCount(docs[i].text)
                    << "}";
            }

            ss << "]";

            res.set_content(
                ss.str(),
                "application/json"
            );
        }
    );

    // ==========================
    // DELETE /doc/delete/:id
    // ==========================

    svr.Delete(
        R"(/doc/delete/(\d+))",
        [&](const httplib::Request& req,
            httplib::Response& res)
        {
            cors(res);

            int id =
                std::stoi(
                    req.matches[1]
                );

            bool ok =
                rag.removeDocument(id);

            if(!ok)
                res.status = 404;

            res.set_content(
                ok
                ? "{\"status\":\"ok\"}"
                : "{\"error\":\"not found\"}",
                "application/json"
            );
        }
    );

    // ==========================
    // POST /doc/search
    // ==========================

    svr.Post(
        "/doc/search",
        [&](const httplib::Request& req,
            httplib::Response& res)
        {
            cors(res);

            std::string question =
                extractStr(
                    req.body,
                    "question"
                );

            if(question.empty())
            {
                res.status = 400;

                res.set_content(
                    "{\"error\":\"missing question\"}",
                    "application/json"
                );

                return;
            }

            int k =
                extractInt(
                    req.body,
                    "k",
                    3
                );

            auto docs =
                rag.retrieve(
                    question,
                    k
                );

            std::ostringstream ss;

            ss << "{\"contexts\":[";

            for(size_t i = 0; i < docs.size(); i++)
            {
                if(i)
                    ss << ",";

                ss
                    << "{"
                    << "\"id\":"
                    << docs[i].id
                    << ","
                    << "\"title\":\""
                    << jsonEscape(docs[i].title)
                    << "\","
                    << "\"text\":\""
                    << jsonEscape(docs[i].text)
                    << "\","
                    << "\"distance\":0"
                    << "}";
            }

            ss << "]}";

            res.set_content(
                ss.str(),
                "application/json"
            );
        }
    );

    // ==========================
    // POST /doc/ask
    // ==========================
    svr.Post(
        "/doc/ask",
        [&](const httplib::Request& req,
            httplib::Response& res)
        {
            cors(res);

            std::string question =
                extractStr(
                    req.body,
                    "question"
                );

            if(question.empty())
            {
                res.status = 400;

                res.set_content(
                    "{\"error\":\"missing question\"}",
                    "application/json"
                );

                return;
            }

            int k =
                extractInt(
                    req.body,
                    "k",
                    3
                );

            std::string answer =
                rag.ask(
                    question,
                    k
                );

            std::ostringstream ss;

            ss
                << "{"
                << "\"answer\":\""
                << jsonEscape(answer)
                << "\","
                << "\"model\":\"local llm\","
                << "\"contexts\":[";

            auto docs =
                rag.retrieve(
                    question,
                    k
                );

            for(size_t i = 0; i < docs.size(); i++)
            {
                if(i)
                    ss << ",";

                ss
                    << "{"
                    << "\"id\":"
                    << docs[i].id
                    << ","
                    << "\"title\":\""
                    << jsonEscape(docs[i].title)
                    << "\","
                    << "\"text\":\""
                    << jsonEscape(docs[i].text)
                    << "\","
                    << "\"distance\":0"
                    << "}";
            }

            ss << "]}";

            res.set_content(
                ss.str(),
                "application/json"
            );
        }
    );

    std::cout
        << "Server running at:\n";

    std::cout
        << "http://localhost:"
        << port
        << "\n";

    svr.listen(
        "0.0.0.0",
        port
    );
}

