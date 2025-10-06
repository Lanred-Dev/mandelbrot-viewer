#pragma once

namespace shaders
{
    const char *mandelbrotFragmentShader = R"(
#version 460 core
out vec4 fColor;
in vec2 uv;

uniform int iterations;
uniform float realMin;
uniform float realMax;
uniform float complexMin;
uniform float complexMax;
uniform float offsetX;
uniform float offsetY;

void main() {
    float cx = uv.x * (realMax - realMin) + realMin + offsetX;
    float cy = uv.y * (complexMax - complexMin) + complexMin + offsetY;

    float zx = 0.0;
    float zy = 0.0;

    int iteration = 0;
    while (zx*zx + zy*zy < 4.0 && iteration < iterations) {
        float temp = zx*zx - zy*zy + cx;
        zy = 2.0 * zx * zy + cy;
        zx = temp;
        iteration++;
    }

    float hue = iteration / iterations;
    float saturation = 1.0;
    float brightness = iteration < iterations ? 1.0 : 0.0;
    float c = brightness * saturation;
    float x = c * (1.0 - abs(mod(hue * 6.0, 2.0) - 1.0));
    float m = brightness - c;

    if (hue < 1.0/6.0) {
        fColor = vec4(c + m, x + m, m, 1.0);
    } else if (hue < 2.0/6.0) {
        fColor = vec4(x + m, c + m, m, 1.0);
    } else if (hue < 3.0/6.0) {
        fColor = vec4(m, c + m, x + m, 1.0);
    } else if (hue < 4.0/6.0) {
        fColor = vec4(m, x + m, c + m, 1.0);
    } else if (hue < 5.0/6.0) {
        fColor = vec4(x + m, m, c + m, 1.0);
    } else {
        fColor = vec4(c + m, m, x + m, 1.0);
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