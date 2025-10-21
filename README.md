# Mandelbrot Viewer

A real-time Mandelbrot set viewer written in C++ using OpenGL.

### Controls

| Key           | Action                                |
| ------------- | ------------------------------------- |
| ↑ / ↓         | Increase / decrease iterations        |
| E / Q         | Zoom in / out                         |
| W / A / S / D | Move around                           |
| R             | Take a screenshot of the current view |

### Building

1. Clone this repository
```bash
git clone https://github.com/Lanred-Dev/mandelbrot.git
cd mandelbrot
```

2. Build using CMake
```bash
cmake -S . -B build
cmake --build build --config Release -j 22
```
