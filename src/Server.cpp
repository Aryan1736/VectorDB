#include "Server.hpp"
#include "DemoData.hpp"
#include "third_party/httplib.h"

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