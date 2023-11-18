#include <iostream>
#include <complex>
#include <future>
#include <vector>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include "lodepng.h"

using namespace std;
namespace fs = std::filesystem;

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

int getUserInput(const char* message) {
    int result;
    cout << message;
    cin >> result;
    return result;
}

void iterationsToColor(int iterations, int maxIterations, uint8_t& r, uint8_t& g, uint8_t& b) {
    double hue = static_cast<double>(iterations) / maxIterations;
    double saturation = 1.0;
    double brightness = (iterations < maxIterations) ? 1.0 : 0.0;

    double c = brightness * saturation;
    double x = c * (1 - abs(fmod(hue * 6.0, 2.0) - 1));
    double m = brightness - c;

    if (hue < 1.0 / 6.0) {
        r = static_cast<uint8_t>((c + m) * 255);
        g = static_cast<uint8_t>((x + m) * 255);
        b = static_cast<uint8_t>(m * 255);
    }
    else if (hue < 2.0 / 6.0) {
        r = static_cast<uint8_t>((x + m) * 255);
        g = static_cast<uint8_t>((c + m) * 255);
        b = static_cast<uint8_t>(m * 255);
    }
    else if (hue < 3.0 / 6.0) {
        r = static_cast<uint8_t>(m * 255);
        g = static_cast<uint8_t>((c + m) * 255);
        b = static_cast<uint8_t>((x + m) * 255);
    }
    else if (hue < 4.0 / 6.0) {
        r = static_cast<uint8_t>(m * 255);
        g = static_cast<uint8_t>((x + m) * 255);
        b = static_cast<uint8_t>((c + m) * 255);
    }
    else if (hue < 5.0 / 6.0) {
        r = static_cast<uint8_t>((x + m) * 255);
        g = static_cast<uint8_t>(m * 255);
        b = static_cast<uint8_t>((c + m) * 255);
    }
    else {
        r = static_cast<uint8_t>((c + m) * 255);
        g = static_cast<uint8_t>(m * 255);
        b = static_cast<uint8_t>((x + m) * 255);
    }
}

void generateMandelbrotRow(int y, int width, int height, int maxIterations, double realMin, double realMax, double complexMin, double complexMax, vector<uint8_t>& pixels) {
    for (int x = 0; x < width; ++x) {
        double xComplex = x * ((realMax - realMin) / (width - 1)) + realMin;
        double yComplex = y * (complexMax - complexMin) / (height - 1) + complexMin;
        complex<double> complexNumber(xComplex, yComplex);
        complex<double> iterationComplex(xComplex, yComplex);
        int iterations = 0;

        while (abs(iterationComplex) < 2.0 && iterations < maxIterations) {
            iterations++;
            iterationComplex = iterationComplex * iterationComplex + complexNumber;
        }

        uint8_t r, g, b;
        iterationsToColor(iterations, maxIterations, r, g, b);

        size_t index = (static_cast<size_t>(y) * width + x) * 4;
        pixels[index] = r;
        pixels[index + 1] = g;
        pixels[index + 2] = b;
        pixels[index + 3] = 255;
    }
}

string generateFilename() {
    auto now = chrono::system_clock::now();
    auto timepoint = chrono::system_clock::to_time_t(now);

    stringstream ss;
    struct tm timeInfo;
    localtime_s(&timeInfo, &timepoint);
    ss << put_time(&timeInfo, "%Y%m%d_%H%M%S");

    return "result_" + ss.str() + ".png";
}

int main() {
    int width = getUserInput("Enter width:");
    int height = getUserInput("Enter height:");
    int maxIterations = getUserInput("Enter iterations:");
    double realMin = getUserInput("Enter real min:");
    double realMax = getUserInput("Enter real max:");
    double complexMax = getUserInput("Enter complex min:");
    double complexMin = getUserInput("Enter complex max:");

    cout << endl;

    vector<uint8_t> pixels(width * height * 4, 0);
    vector<future<void>> futures;

    for (int y = 0; y < height; ++y) {
        futures.push_back(async(launch::async, generateMandelbrotRow, y, width, height, maxIterations, realMin, realMax, complexMin, complexMax, ref(pixels)));
        updateProgressBar(y, height);
    }

    for (auto& future : futures) {
        future.wait();
    }

    string filename = generateFilename();

    if (lodepng::encode(filename.c_str(), pixels, width, height) == 0) {
        cout << endl << "Generated: " << filename << endl;
        system(("start " + filename).c_str());
    }

    return 0;
}