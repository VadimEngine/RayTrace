cmake_minimum_required(VERSION 3.12)

project(OpenGLTutorial)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CLAY_ENABLE_OPENGL ON) # Enable Opengl

# Collect all source files
file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Add the executable
add_executable(${PROJECT_NAME} ${SRC_FILES})
# Define the DEMO_RESOURCE_PATH macro
target_compile_definitions(${PROJECT_NAME} PRIVATE DEMO_RESOURCE_PATH="${CMAKE_SOURCE_DIR}/res")


# Add ClayEngine as a subdirectory and link to it
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glfw)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glew-cmake)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/assimp)


target_link_libraries(${PROJECT_NAME} PUBLIC 
    opengl32
    winmm
    avrt
    libglew_static
    glfw
    assimp
)

# Include project-specific directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/stb
)
