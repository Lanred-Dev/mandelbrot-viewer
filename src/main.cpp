#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <fstream>
#include "shaders.h"
#include "utils/input.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const char *WINDOW_TITLE = "Mandelbrot";
const float ZOOM_FACTOR = 0.95f;
int iterations;
float realMin, realMax;
float complexMin, complexMax;
float offsetX = 0.0f;
float offsetY = 0.0f;

void shiftView(float dx, float dy)
{
    offsetX += dx * (realMax - realMin);
    offsetY += dy * (complexMax - complexMin);
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
        std::cout << "Iterations: " << iterations << "\n";
        break;
    }
    case GLFW_KEY_DOWN:
    {
        iterations = std::max(50, iterations - 50);
        std::cout << "Iterations: " << iterations << "\n";
        break;
    }

    case GLFW_KEY_E:
    {
        float centerReal = (realMin + realMax) / 2.0f;
        float centerComplex = (complexMin + complexMax) / 2.0f;
        float realRange = (realMax - realMin) * ZOOM_FACTOR;
        float complexRange = (complexMax - complexMin) * ZOOM_FACTOR;
        realMin = centerReal - realRange / 2.0f;
        realMax = centerReal + realRange / 2.0f;
        complexMin = centerComplex - complexRange / 2.0f;
        complexMax = centerComplex + complexRange / 2.0f;
        break;
    }

    case GLFW_KEY_Q:
    {
        float centerReal = (realMin + realMax) / 2.0f;
        float centerComplex = (complexMin + complexMax) / 2.0f;
        float realRange = (realMax - realMin) / ZOOM_FACTOR;
        float complexRange = (complexMax - complexMin) / ZOOM_FACTOR;
        realMin = centerReal - realRange / 2.0f;
        realMax = centerReal + realRange / 2.0f;
        complexMin = centerComplex - complexRange / 2.0f;
        complexMax = centerComplex + complexRange / 2.0f;
        break;
    }

    case GLFW_KEY_A:
    {
        shiftView(-0.1f, 0.0f);
        break;
    }
    case GLFW_KEY_D:
    {
        shiftView(0.1f, 0.0f);
        break;
    }
    case GLFW_KEY_W:
    {
        shiftView(0.0f, 0.1f);
        break;
    }
    case GLFW_KEY_S:
    {
        shiftView(0.0f, -0.1f);
        break;
    }

    case GLFW_KEY_R:
    {
        std::vector<unsigned char> pixels(WINDOW_WIDTH * WINDOW_HEIGHT * 3);
        glReadPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

        std::vector<unsigned char> bmpPixels(WINDOW_WIDTH * WINDOW_HEIGHT * 3);
        for (int y = 0; y < WINDOW_HEIGHT; y++)
        {
            for (int x = 0; x < WINDOW_WIDTH * 3; x++)
            {
                bmpPixels[(y * WINDOW_WIDTH * 3) + x] = pixels[((WINDOW_HEIGHT - 1 - y) * WINDOW_WIDTH * 3) + x];
            }
        }

        // BMP header
        unsigned char header[54] = {0};
        int fileSize = 54 + WINDOW_WIDTH * WINDOW_HEIGHT * 3;
        header[0] = 'B';
        header[1] = 'M';
        header[2] = fileSize;
        header[3] = fileSize >> 8;
        header[4] = fileSize >> 16;
        header[5] = fileSize >> 24;
        header[10] = 54;
        header[14] = 40;
        header[18] = WINDOW_WIDTH;
        header[19] = WINDOW_WIDTH >> 8;
        header[20] = WINDOW_WIDTH >> 16;
        header[21] = WINDOW_WIDTH >> 24;
        header[22] = WINDOW_HEIGHT;
        header[23] = WINDOW_HEIGHT >> 8;
        header[24] = WINDOW_HEIGHT >> 16;
        header[25] = WINDOW_HEIGHT >> 24;
        header[26] = 1;
        header[28] = 24;

        std::ofstream out("screenshot.bmp", std::ios::binary);
        out.write((char *)header, 54);
        out.write((char *)bmpPixels.data(), WINDOW_WIDTH * WINDOW_HEIGHT * 3);
        out.close();
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

int main()
{
    iterations = (int)input("Enter iterations:");
    realMin = input("Enter real min:");
    realMax = input("Enter real max:");
    complexMin = input("Enter complex min:");
    complexMax = input("Enter complex max:");

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
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
    if (!success)
    {
        char info[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, info);
        std::cerr << "Shader program link error:\n"
                  << info << "\n";
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glfwSetKeyCallback(window, keyCallback);

    GLint iterationsLoc = glGetUniformLocation(shaderProgram, "iterations");
    GLint realMinLoc = glGetUniformLocation(shaderProgram, "realMin");
    GLint realMaxLoc = glGetUniformLocation(shaderProgram, "realMax");
    GLint complexMinLoc = glGetUniformLocation(shaderProgram, "complexMin");
    GLint complexMaxLoc = glGetUniformLocation(shaderProgram, "complexMax");
    GLint offsetXLoc = glGetUniformLocation(shaderProgram, "offsetX");
    GLint offsetYLoc = glGetUniformLocation(shaderProgram, "offsetY");

    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform1i(iterationsLoc, iterations);
        glUniform1f(realMinLoc, realMin);
        glUniform1f(realMaxLoc, realMax);
        glUniform1f(complexMinLoc, complexMin);
        glUniform1f(complexMaxLoc, complexMax);
        glUniform1f(offsetXLoc, offsetX);
        glUniform1f(offsetYLoc, offsetY);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
