# VoxelEngine
This is an ongoing graduation project. The focus is creating a Voxel engine with cellular automata style physics. It will be finished in early May 2026.

*In the future, a link will be put here for the progress documentation.*

## Installation

Dependencies:
- [GLFW 3.4](https://github.com/glfw/glfw/tree/3.4)
- [glad (gl=4.6)](https://glad.dav1d.de/)
- [glm 1.0.3](https://github.com/g-truc/glm/tree/1.0.3)
- [imgui v1.92.6-docking](https://github.com/ocornut/imgui/tree/v1.92.6-docking)
- [nlohmann/json v3.12.0](https://github.com/nlohmann/json/tree/v3.12.0)

Most dependencies are automatically pulled with CMake's FetchContent, so it will be fetched during compilation.

Requirements:
- C++23
- CMake 3.16


I am using Ninja for the build generator, but you can use whatever you please. Here are the very simple instructions on cloning and building.
```
git clone https://github.com/SolidSoups/VoxelEngine.git
cd VoxelEngine
mkdir build
cmake -B build -S . -G Ninja 
ninja -C build
```
