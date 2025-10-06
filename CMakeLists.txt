cmake_minimum_required(VERSION 3.16)
project(mandelbrot)

set(CMAKE_CXX_STANDARD 17)

add_library(glad STATIC lib/glad.c)
target_include_directories(glad PUBLIC include)

add_executable(mandelbrot src/main.cpp)

target_include_directories(mandelbrot PRIVATE include)

target_link_libraries(mandelbrot PRIVATE glad ${CMAKE_SOURCE_DIR}/lib/glfw3.lib opengl32 gdi32)
