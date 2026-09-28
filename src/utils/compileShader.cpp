#include "compileShader.hpp"
#include <windows.h>

std::tuple<int, GLuint> utils::compileShader(GLuint *shaderProgram, GLenum shaderType, const int resourceId)
{
    HRSRC hRes = FindResourceA(nullptr, MAKEINTRESOURCEA(resourceId), (LPCSTR)RT_RCDATA);

    if (!hRes)
    {
        MessageBoxA(nullptr, "Failed to find shader resource", "Error", MB_OK | MB_ICONERROR);
        return {EXIT_FAILURE, 0};
    }

    HGLOBAL hMem = LoadResource(nullptr, hRes);
    DWORD size = SizeofResource(nullptr, hRes);
    const char *data = static_cast<const char *>(LockResource(hMem));

    if (!data)
    {
        MessageBoxA(nullptr, "Failed to lock shader resource", "Error", MB_OK | MB_ICONERROR);
        return {EXIT_FAILURE, 0};
    }

    GLuint shader = glCreateShader(shaderType);
    GLint shaderLength = static_cast<GLint>(size);
    glShaderSource(shader, 1, &data, &shaderLength);
    glCompileShader(shader);

    GLint didCompile = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &didCompile);

    if (!didCompile)
    {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        MessageBoxA(nullptr, infoLog, "GLSL Compute Shader Compile Error", MB_OK | MB_ICONERROR);
        glDeleteShader(shader);
        return {EXIT_FAILURE, 0};
    }

    glAttachShader(*shaderProgram, shader);
    return {EXIT_SUCCESS, shader};
}