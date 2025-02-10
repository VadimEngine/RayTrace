#pragma once
// standard lib
#include <string>
#include <stdexcept>
// third party
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
// project
#include "core/application/InputHandler.h"

class Window {
public:
    /** Constructor */
    Window(const std::string& windowLbl = "", int width = SCR_WIDTH, int height = SCR_HEIGHT);

    /** Destructor */
    ~Window();

    /**
     * Update the window (mainly polls for inputs)
     * @param dt Time since last update in seconds
     */
    void update(float dt);

    /** Render the window content */
    void render();

    /**
     * @brief Set if the window should be displayed
     */
    void enableDisplay(bool show);

    /** Get the GLFW window */
    GLFWwindow* getGLFWWindow() const;

    /** If the Window is currently running */
    bool isRunning() const;

    /**
     * Enable/Disable VSync
     * @param enabled VSync status
     */
    void setVSync(const bool enabled);

    /** Get the GLFW swap interval. 0 means disabled, 1 means enabled */
    int getGLFWSwapInterval() const;

    /** Get the screen dimension of this window*/
    glm::ivec2 getDimensions() const;

    /** Get the input handler listening to this Window's inputs */
    InputHandler* getInputHandler();

private:
    /** Default Screen Width */
    static constexpr unsigned int SCR_WIDTH = 800;
    /** Default Screen Height */
    static constexpr unsigned int SCR_HEIGHT = 800;
    /** GLFW window this window wraps */
    GLFWwindow* mpGLFWWindow_ = nullptr;
    /** Input handler listening to inputs on this window*/
    InputHandler mInputHandler_;
    /** GLFW swap interval for VSync */
    unsigned int mSwapInterval_ = 1;
};
