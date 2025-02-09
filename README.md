Introduction to ray tracing with OpenGL with Compute Shaders. Use tab key to swap scenes

# Instructions

Third party libraries must be added a ./thirdparty folder (or update theCMake)
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


![alt text](./screenshots/RayTrace1.png)
