#include <iostream>
#include "iterationsToColor.h"

using namespace std;

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