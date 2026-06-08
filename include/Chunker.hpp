#pragma once

#include <vector>
#include <string>

class Chunker
{
public:

    static std::vector<std::string>
    chunkText(
        const std::string& text,
        int chunkWords = 200,
        int overlapWords = 30
    );
};