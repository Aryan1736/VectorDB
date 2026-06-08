#pragma once

#include <vector>
#include <string>

class Embedder
{
public:

    static std::vector<float>
    embed(
        const std::string& text,
        int dims = 128
    );
};