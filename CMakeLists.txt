cmake_minimum_required(VERSION 3.16)
project(mandelbrot)

set(CMAKE_CXX_STANDARD 17)

add_library(glfw3 STATIC IMPORTED)
set_target_properties(glfw3 PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/glfw3.lib"
)

add_executable(mandelbrot src/main.cpp lib/glad.c)
target_include_directories(mandelbrot PRIVATE include)

target_link_libraries(mandelbrot PRIVATE glfw3 opengl32 gdi32)
