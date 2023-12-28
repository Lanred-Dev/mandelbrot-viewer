#include <iostream>
#include <iomanip>
#include "progressBar.h"

using namespace std;

void updateProgressBar(int progress, int total) {
    const int barWidth = 50;
    float progressRatio = static_cast<float>(progress) / total;
    int barProgress = static_cast<int>(barWidth * progressRatio) + 1;

    cout << "[";

    for (int i = 0; i < barWidth; ++i) {
        if (i < barProgress) {
            cout << "=";
        }
        else {
            cout << " ";
        }
    }

    cout << "] " << setw(3) << static_cast<int>(progressRatio * 100.0) + 1 << "%\r";
    cout.flush();
}