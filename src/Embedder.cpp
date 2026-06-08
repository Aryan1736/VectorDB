#include "Embedder.hpp"

#include <functional>

std::vector<float>
Embedder::embed(
    const std::string& text,
    int dims
)
{
    std::vector<float> vec(
        dims,
        0.0f
    );

    std::hash<std::string> hasher;

    std::string word;

    for(char c : text)
    {
        if(c == ' ')
        {
            if(!word.empty())
            {
                size_t h =
                    hasher(word);

                vec[h % dims] += 1.0f;

                word.clear();
            }
        }
        else
        {
            word += c;
        }
    }

    if(!word.empty())
    {
        size_t h =
            hasher(word);

        vec[h % dims] += 1.0f;
    }

    return vec;
}