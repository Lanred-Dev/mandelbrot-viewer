#include <iostream>
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include "generateName.h"

using namespace std;

string generateName() {
    auto now = chrono::system_clock::now();
    auto timepoint = chrono::system_clock::to_time_t(now);

    stringstream ss;
    struct tm timeInfo;
    localtime_s(&timeInfo, &timepoint);
    ss << put_time(&timeInfo, "%Y%m%d_%H%M%S");

    return "result_" + ss.str();
}