cmake_minimum_required(VERSION 3.12)

project(OpenGLTutorial)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Collect all source files
file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Add imgui source files
list(APPEND SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/backends/imgui_impl_opengl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/backends/imgui_impl_glfw.cpp
)

# Add the executable
add_executable(${PROJECT_NAME} ${SRC_FILES})
# Define the RESOURCE_PATH macro
target_compile_definitions(${PROJECT_NAME} PRIVATE RESOURCE_PATH="${CMAKE_SOURCE_DIR}/res")

# TODO compile flags

# Add ClayEngine as a subdirectory and link to it
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glfw)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glew-cmake)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/assimp)

set(GLFW_USE_DWM_SWAP_INTERVAL ON CACHE BOOL "Set swap interval even when DWM compositing is enabled" FORCE)


target_link_libraries(${PROJECT_NAME} PUBLIC 
    opengl32
    winmm
    avrt
    libglew_static
    glfw
    assimp
)
set(GLFW_USE_DWM_SWAP_INTERVAL ON CACHE BOOL "Set swap interval even when DWM compositing is enabled" FORCE)

# Include project-specific directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/backends
)
