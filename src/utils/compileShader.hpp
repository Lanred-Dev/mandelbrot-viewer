#pragma once

#include <glad/glad.h>
#include <tuple>

namespace utils {
    std::tuple<int, GLuint> compileShader(GLuint *shaderProgram, GLenum shaderType, const int resourceId);
}