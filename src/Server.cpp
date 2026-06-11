#include "Server.hpp"
#include "DemoData.hpp"
#include "third_party/httplib.h"
#include "JsonUtils.hpp"

#include <iostream>
#include <sstream>

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
                << docDB.size()
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
                    << items[i].metadata
                    << "\","
                    << "\"category\":\""
                    << items[i].category
                    << "\""
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
                    << h.metadata
                    << "\","
                    << "\"category\":\""
                    << h.category
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
                << "\"kdtree_us\":"
                << out.kdUs
                << ","
                << "\"hnsw_us\":"
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
                << "}";

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

