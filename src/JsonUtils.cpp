#include "JsonUtils.hpp"

#include <sstream>

std::vector<float> parseVec(
    const std::string& s
)
{
    std::vector<float> v;

    std::stringstream ss(s);

    std::string token;

    while(std::getline(ss, token, ','))
    {
        try
        {
            v.push_back(
                std::stof(token)
            );
        }
        catch(...)
        {
        }
    }

    return v;
}

std::string extractStr(
    const std::string& body,
    const std::string& key
)
{
    size_t p =
        body.find(
            "\"" + key + "\""
        );

    if(p == std::string::npos)
        return "";

    p = body.find(':', p);

    if(p == std::string::npos)
        return "";

    p++;

    while(
        p < body.size()
        &&
        body[p] != '"'
    )
        p++;

    p++;

    std::string result;

    while(
        p < body.size()
        &&
        body[p] != '"'
    )
    {
        result += body[p++];
    }

    return result;
}