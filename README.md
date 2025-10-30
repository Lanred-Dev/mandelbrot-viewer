# Mandelbrot Viewer

A real-time Mandelbrot set viewer written in C++ using OpenGL.

### Controls

| Key           | Action                                |
| ------------- | ------------------------------------- |
| ↑ / ↓         | Increase / decrease iterations        |
| E / Q         | Zoom in / out                         |
| W / A / S / D | Move around                           |
| R             | Take a screenshot of the current view |
| Escape        | Close the application                 |

### Media

![Screenshot](./media/screenshot1.png)
![Screenshot](./media/screenshot2.png)
![Screenshot](./media/screenshot3.png)

### Requirements

- Windows operating system
- C++23 (or newer) compiler *
- CMake 3.16 (or newer) *

* Required only if building from source

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
