Introduction to ray tracing with OpenGL with Compute Shaders

# Instructions

Third party libraries must be added a ./thirdparty folder (or update theCMake)
- assimp
- glew-cmake
- glfw
- glm
- stb

## Build commands

- To build (outputs to `/build` directory):
    - `git submodule update --init --recursive`
    - `cmake -S . -B build`
    - `cmake --build ./build/`
- To run:
    - `./build/Debug/OpenGLTutorial.exe`


![alt text](./screenshots/RayTrace1.png)
