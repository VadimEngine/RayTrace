Introduction to ray tracing with OpenGL with Compute Shaders. Use tab key to swap scenes.

Also includes other demo scenes to experiment with different rendering techniques

# Instructions

Third party libraries must be added a ./thirdparty folder (or update the CMake to point to these third party libs)
- assimp
- glew-cmake
- glfw (Currently using 3.3.8 to allow enabling vysnc with glfwSwapInterval(1))
- glm
- stb
- imgui

## Build commands

- To build (outputs to `/build` directory):
    - `git submodule update --init --recursive`
    - `cmake -S . -B build`
    - `cmake --build ./build/`
- To run:
    - `./build/Debug/OpenGLTutorial.exe`


# Ray Trace Scene
Ray tracing using Compute Shaders

![alt text](./screenshots/RayTrace1.png)

# Stencil Scene
Highlight objects with Stencil Shader

![alt text](./screenshots/Stencil.PNG)

# AABB Visualize Scene
Visualize a collection of AABBs (Axis Aligned Bounding Box)

![alt text](./screenshots/AABB.PNG)