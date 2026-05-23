#pragma once

namespace shaders
{
    const char *mandelbrotFragmentShader = R"(
#version 460 core
out vec4 fColor;
in vec2 uv;

uniform int iterations;
uniform double realMin;
uniform double realMax;
uniform double complexMin;
uniform double complexMax;

void main() {
    double cx = double(uv.x) * (realMax - realMin) + realMin;
    double cy = double(uv.y) * (complexMax - complexMin) + complexMin;

    double zx = 0.0;
    double zy = 0.0;

    int iteration = 0;
    while (zx*zx + zy*zy < 4.0 && iteration < iterations) {
        double temp = zx*zx - zy*zy + cx;
        zy = 2.0 * zx * zy + cy;
        zx = temp;
        iteration++;
    }

    float t = float(iteration) / float(iterations);

    if (iteration == iterations) {
        fColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
        float r = 0.5 + 0.5 * cos(3.1415926 * 2.0 * (t * 5.0 + 0.0));
        float g = 0.5 + 0.5 * cos(3.1415926 * 2.0 * (t * 5.0 + 0.15));
        float b = 0.5 + 0.5 * cos(3.1415926 * 2.0 * (t * 5.0 + 0.30));

        fColor = vec4(r, g, b, 1.0);
    }
}
)";

    const char *vertexShader = R"(
#version 460 core
layout(location = 0) in vec2 aPos;
out vec2 uv;
void main() {
    uv = aPos * 0.5 + 0.5;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
    )";
}