# VoxelEngine
This is an ongoing graduation project. The focus is creating a Voxel engine with cellular automata style physics. It will be finished in early May 2026.

*In the future, a link will be put here for the progress documentation.*

## Installation

Dependencies (mostly automatically pulled with CMake's FetchContent):
- [GLFW 3.4](https://github.com/glfw/glfw.git/tree/3.4)
- 


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
