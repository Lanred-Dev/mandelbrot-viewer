#include <iostream>
#include "getUserInput.h"

using namespace std;

float getUserInput(const char* message) {
    float result;
    std::cout << message;
    std::cin >> result;

    if (std::floor(result) == result) {
        return static_cast<int>(result);
    }
    else {
        return result;
    }
}