cmake_minimum_required(VERSION 3.16)
project(mandelbrot LANGUAGES C CXX RC)

set(CMAKE_CXX_STANDARD 23)

add_library(glfw3 STATIC IMPORTED)
set_target_properties(glfw3 PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/glfw3.lib"
)

add_executable(mandelbrot src/main.cpp lib/glad.c src/resources.rc src/utils/compileShader.cpp src/core/systems/InputManager.cpp)

target_include_directories(mandelbrot PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(mandelbrot PRIVATE glfw3 opengl32 gdi32)
set_target_properties(mandelbrot PROPERTIES WIN32_EXECUTABLE TRUE)