#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <string>
#include <windows.h>
#include "shaders.h"

const char *WINDOW_TITLE = "Mandelbrot Viewer";
const double ZOOM_FACTOR = 0.8;

int windowWidth = 800;
int windowHeight = 600;
int iterations = 500;
double realMin = -2.0;
double realMax = 2.0;
double complexMin = -1.5;
double complexMax = 1.5;

void shiftView(double dx, double dy)
{
    double realSpan = realMax - realMin;
    double complexSpan = complexMax - complexMin;

    realMin += dx * realSpan;
    realMax += dx * realSpan;
    complexMin += dy * complexSpan;
    complexMax += dy * complexSpan;
}

void zoomView(double zoomFactor)
{
    double centerReal = (realMin + realMax) / 2.0;
    double centerComplex = (complexMin + complexMax) / 2.0;
    double realRange = (realMax - realMin) * zoomFactor;
    double complexRange = (complexMax - complexMin) * zoomFactor;

    realMin = centerReal - realRange / 2.0;
    realMax = centerReal + realRange / 2.0;
    complexMin = centerComplex - complexRange / 2.0;
    complexMax = centerComplex + complexRange / 2.0;
}

void captureScreenshot(const char *filename, GLFWwindow *window)
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

    system(("start " + std::string(filename)).c_str());
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;

    switch (key)
    {
    case GLFW_KEY_ESCAPE:
    {
        glfwSetWindowShouldClose(window, true);
        break;
    }

    case GLFW_KEY_UP:
    {
        iterations += 50;
        break;
    }
    case GLFW_KEY_DOWN:
    {
        iterations = std::max<int>(50, iterations - 50);
        break;
    }

    case GLFW_KEY_E:
    {
        zoomView(ZOOM_FACTOR);
        break;
    }

    case GLFW_KEY_Q:
    {
        zoomView(1.0 / ZOOM_FACTOR);
        break;
    }

    case GLFW_KEY_A:
    {
        shiftView(-0.1, 0.0);
        break;
    }
    case GLFW_KEY_D:
    {
        shiftView(0.1, 0.0);
        break;
    }
    case GLFW_KEY_W:
    {
        shiftView(0.0, 0.1);
        break;
    }
    case GLFW_KEY_S:
    {
        shiftView(0.0, -0.1);
        break;
    }

    case GLFW_KEY_R:
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        std::string filename = "screenshot_" + std::to_string(time) + ".bmp";
        captureScreenshot(filename.c_str(), window);
        break;
    }

    default:
        break;
    }
}

GLuint compileShader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char info[512];
        glGetShaderInfoLog(shader, 512, nullptr, info);
        std::cerr << "Shader compile error:\n"
                  << info << "\n";
    }

    return shader;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
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

    float vertices[] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, shaders::vertexShader);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, shaders::mandelbrotFragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (!success)
    {
        char info[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, info);
        MessageBoxA(nullptr, info, "Shader Program Link Error", MB_OK | MB_ICONERROR);
        glDeleteProgram(shaderProgram);
        glfwDestroyWindow(window);
        glfwTerminate();

        return EXIT_FAILURE;
    }

    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    GLint iterationsLocation = glGetUniformLocation(shaderProgram, "iterations");
    GLint realMinLocation = glGetUniformLocation(shaderProgram, "realMin");
    GLint realMaxLocation = glGetUniformLocation(shaderProgram, "realMax");
    GLint complexMinLocation = glGetUniformLocation(shaderProgram, "complexMin");
    GLint complexMaxLocation = glGetUniformLocation(shaderProgram, "complexMax");

    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    glViewport(0, 0, windowWidth, windowHeight);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform1i(iterationsLocation, iterations);
        glUniform1d(realMinLocation, realMin);
        glUniform1d(realMaxLocation, realMax);
        glUniform1d(complexMinLocation, complexMin);
        glUniform1d(complexMaxLocation, complexMax);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
