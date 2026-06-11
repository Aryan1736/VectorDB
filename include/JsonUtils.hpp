#pragma once

#include <string>
#include <vector>

std::vector<float> parseVec(
    const std::string& s
);

std::string extractStr(
    const std::string& body,
    const std::string& key
);