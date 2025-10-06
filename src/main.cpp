#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <fstream>
#include "shaders.h"

int window_width = 800;
int window_height = 600;
const char *WINDOW_TITLE = "Mandelbrot";
const float ZOOM_FACTOR = 0.95f;
int iterations = 500;
float realMin = -2.0f;
float realMax = 2.0f;
float complexMin = -1.5f;
float complexMax = 1.5f;
float offsetX = 0.0f;
float offsetY = 0.0f;

void shiftView(float dx, float dy)
{
    offsetX += dx * (realMax - realMin);
    offsetY += dy * (complexMax - complexMin);
}

void captureScreenshot(const char *filename, GLFWwindow *window)
{
    std::vector<unsigned char> pixels(window_width * window_height * 3);
    glReadPixels(0, 0, window_width, window_height, GL_BGR, GL_UNSIGNED_BYTE, pixels.data());

    unsigned char header[54] = {0};
    int fileSize = 54 + window_width * window_height * 3;
    header[0] = 'B';
    header[1] = 'M';
    header[2] = fileSize;
    header[3] = fileSize >> 8;
    header[4] = fileSize >> 16;
    header[5] = fileSize >> 24;
    header[10] = 54;
    header[14] = 40;
    header[18] = window_width;
    header[19] = window_width >> 8;
    header[20] = window_width >> 16;
    header[21] = window_width >> 24;
    header[22] = window_height;
    header[23] = window_height >> 8;
    header[24] = window_height >> 16;
    header[25] = window_height >> 24;
    header[26] = 1;
    header[28] = 24;

    std::ofstream out(filename, std::ios::binary);
    out.write((char *)header, 54);
    out.write((char *)pixels.data(), window_width * window_height * 3);
    out.close();

#ifdef _WIN32
    system(("start " + std::string(filename)).c_str());
#endif
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
        captureScreenshot("screenshot.bmp", window);
        std::cout << "Screenshot saved as screenshot.bmp\n";
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
    window_height = height;
    window_width = width;
    glViewport(0, 0, width, height);
}

int main()
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(window_width, window_height, WINDOW_TITLE, nullptr, nullptr);
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
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    GLint iterationsLoc = glGetUniformLocation(shaderProgram, "iterations");
    GLint realMinLoc = glGetUniformLocation(shaderProgram, "realMin");
    GLint realMaxLoc = glGetUniformLocation(shaderProgram, "realMax");
    GLint complexMinLoc = glGetUniformLocation(shaderProgram, "complexMin");
    GLint complexMaxLoc = glGetUniformLocation(shaderProgram, "complexMax");
    GLint offsetXLoc = glGetUniformLocation(shaderProgram, "offsetX");
    GLint offsetYLoc = glGetUniformLocation(shaderProgram, "offsetY");

    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    glViewport(0, 0, window_width, window_height);

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
