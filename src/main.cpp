#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <string>
#include <sstream>
#include <windows.h>
#include "resource.h"
#include "utils/compileShader.hpp"
#include "core/systems/InputManager.hpp"

const char *WINDOW_TITLE = "Mandelbrot Viewer";
const double ZOOM_FACTOR = 0.8;

int windowWidth = 800;
int windowHeight = 600;
GLuint computeTexture = 0;
Core::Systems::InputManager inputManager;

struct Data
{
    double realMin;
    double realMax;
    double complexMin;
    double complexMax;
    int iterations;
    float colorFrequency;
    int colorMode;
    int padding[3];
};

Data *sharedData = nullptr;

void resizeComputeTexture(int width, int height)
{
    if (computeTexture != 0)
    {
        glDeleteTextures(1, &computeTexture);
    }

    glGenTextures(1, &computeTexture);
    glBindTexture(GL_TEXTURE_2D, computeTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void shiftView(double dx, double dy)
{
    double realSpan = sharedData->realMax - sharedData->realMin;
    double complexSpan = sharedData->complexMax - sharedData->complexMin;
    sharedData->realMin += dx * realSpan;
    sharedData->realMax += dx * realSpan;
    sharedData->complexMin += dy * complexSpan;
    sharedData->complexMax += dy * complexSpan;
}

void zoomView(double zoomFactor)
{
    double centerReal = (sharedData->realMin + sharedData->realMax) / 2.0;
    double centerComplex = (sharedData->complexMin + sharedData->complexMax) / 2.0;
    double realRange = (sharedData->realMax - sharedData->realMin) * zoomFactor;
    double complexRange = (sharedData->complexMax - sharedData->complexMin) * zoomFactor;
    sharedData->realMin = centerReal - realRange / 2.0;
    sharedData->realMax = centerReal + realRange / 2.0;
    sharedData->complexMin = centerComplex - complexRange / 2.0;
    sharedData->complexMax = centerComplex + complexRange / 2.0;
}

void SaveScreenshot(const char *filename, GLFWwindow *window)
{
    std::vector<unsigned char> pixels(windowWidth * windowHeight * 3);
    glReadPixels(0, 0, windowWidth, windowHeight, GL_BGR, GL_UNSIGNED_BYTE, pixels.data());

    unsigned char header[54] = {0};
    int fileSize = 54 + windowWidth * windowHeight * 3;
    header[0] = 'B';
    header[1] = 'M';
    header[2] = fileSize;
    header[3] = fileSize >> 8;
    header[4] = fileSize >> 16;
    header[5] = fileSize >> 24;
    header[10] = 54;
    header[14] = 40;
    header[18] = windowWidth;
    header[19] = windowWidth >> 8;
    header[20] = windowWidth >> 16;
    header[21] = windowWidth >> 24;
    header[22] = windowHeight;
    header[23] = windowHeight >> 8;
    header[24] = windowHeight >> 16;
    header[25] = windowHeight >> 24;
    header[26] = 1;
    header[28] = 24;

    std::ofstream out(filename, std::ios::binary);
    out.write((char *)header, 54);
    out.write((char *)pixels.data(), windowWidth * windowHeight * 3);
    out.close();

    MessageBoxA(nullptr, (std::string("Screenshot saved to ") + std::string(filename)).c_str(), "Success", MB_OK | MB_ICONINFORMATION);
}

void CaptureScreenshot()
{
    auto now = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    std::string filename = "screenshot_" + std::to_string(time) + ".bmp";
    SaveScreenshot(filename.c_str(), glfwGetCurrentContext());
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
    resizeComputeTexture(width, height);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    if (!glfwInit())
    {
        MessageBoxA(nullptr, "Failed to initialize GLFW", "Error", MB_OK | MB_ICONERROR);
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, WINDOW_TITLE, nullptr, nullptr);

    if (!window)
    {
        MessageBoxA(nullptr, "Failed to create GLFW window", "Error", MB_OK | MB_ICONERROR);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        MessageBoxA(nullptr, "Failed to initialize GLAD", "Error", MB_OK | MB_ICONERROR);
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    resizeComputeTexture(windowWidth, windowHeight);

    float vertices[] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    GLuint computeProgram = glCreateProgram();
    const auto [didComputeShaderCompile, computeShader] = utils::compileShader(&computeProgram, GL_COMPUTE_SHADER, IDR_COMPUTE_SHADER);

    if (didComputeShaderCompile == EXIT_FAILURE)
    {
        MessageBoxA(nullptr, "Failed to compile compute shader", "Error", MB_OK | MB_ICONERROR);
        return EXIT_FAILURE;
    }

    glLinkProgram(computeProgram);
    glDeleteShader(computeShader);

    GLuint renderProgram = glCreateProgram();

    const auto [didVertexShaderCompile, vertexShader] = utils::compileShader(&renderProgram, GL_VERTEX_SHADER, IDR_VERTEX_SHADER);

    if (didVertexShaderCompile == EXIT_FAILURE)
    {
        MessageBoxA(nullptr, "Failed to compile vertex shader", "Error", MB_OK | MB_ICONERROR);
        return EXIT_FAILURE;
    }

    const auto [didFragmentShaderCompile, fragmentShader] = utils::compileShader(&renderProgram, GL_FRAGMENT_SHADER, IDR_FRAGMENT_SHADER);

    if (didFragmentShaderCompile == EXIT_FAILURE)
    {
        MessageBoxA(nullptr, "Failed to compile fragment shader", "Error", MB_OK | MB_ICONERROR);
        return EXIT_FAILURE;
    }

    glLinkProgram(renderProgram);
    glDeleteShader(computeShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    inputManager.Initialize(window);

    inputManager.onKeyPressed.Connect([](int key, int mods)
                                      {
        switch (key)
        {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
                break;
            case GLFW_KEY_SPACE:
                CaptureScreenshot();
                break;
            case GLFW_KEY_C:
                sharedData->colorMode = (sharedData->colorMode + 1) % 2;
                break;
            default:
                break;
        } });

    inputManager.onKeyHeld.Connect([](int key, int mods)
                                   {
        switch (key)
        {
            case GLFW_KEY_UP:
                sharedData->iterations += 50;
                break;
            case GLFW_KEY_DOWN:
                sharedData->iterations = std::max<int>(50, sharedData->iterations - 50);
                break;
            case GLFW_KEY_E:
                zoomView(ZOOM_FACTOR);
                break;
            case GLFW_KEY_Q:
                zoomView(1.0 / ZOOM_FACTOR);
                break;
            case GLFW_KEY_W:
                shiftView(0.0, 0.1);
                break;
            case GLFW_KEY_S:
                shiftView(0.0, -0.1);
                break;
            case GLFW_KEY_A:
                shiftView(-0.1, 0.0);
                break;
            case GLFW_KEY_D:
                shiftView(0.1, 0.0);
                break;
            case GLFW_KEY_RIGHT:
                sharedData->colorFrequency += mods & GLFW_MOD_SHIFT ? 0.5f : 0.1f;
                break;
            case GLFW_KEY_LEFT:
                sharedData->colorFrequency = std::max<float>(0.1f, sharedData->colorFrequency - (mods & GLFW_MOD_SHIFT ? 0.5f : 0.1f));
                break;
            default:
                break;
        } });

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    GLuint ubo;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
    glBufferStorage(GL_UNIFORM_BUFFER, sizeof(Data), nullptr, flags);
    sharedData = (Data *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(Data), flags);
    sharedData->realMin = -2.0;
    sharedData->realMax = 2.0;
    sharedData->complexMin = -1.5;
    sharedData->complexMax = 1.5;
    sharedData->iterations = 500;
    sharedData->colorFrequency = 3.0f;
    sharedData->colorMode = 0;
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);

    glViewport(0, 0, windowWidth, windowHeight);

    while (!glfwWindowShouldClose(window))
    {
        glUseProgram(computeProgram);
        glBindImageTexture(0, computeTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        unsigned int xGroups = (windowWidth + 15) / 16;
        unsigned int yGroups = (windowHeight + 15) / 16;
        glDispatchCompute(xGroups, yGroups, 1);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(renderProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, computeTexture);
        glUniform1i(glGetUniformLocation(renderProgram, "renderTexture"), 0);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}