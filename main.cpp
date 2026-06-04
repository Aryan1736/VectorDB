#include <iostream>
#include "Distance.hpp"

int main() {
    std::vector<float> a = {1,2,3};
    std::vector<float> b = {2,3,4};

    std::cout << euclidean(a,b) << '\n';

    return 0;
}