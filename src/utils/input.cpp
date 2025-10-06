#include <iostream>
#include "input.h"

float input(const char *text)
{
    float result;
    std::cout << text;
    std::cin >> result;

    if (std::floor(result) == result)
    {
        return static_cast<int>(result);
    }
    else
    {
        return result;
    }
}