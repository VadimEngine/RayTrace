#include "Window.h"
#include <iostream>

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
   Window* windowWrapper = static_cast<Window*>(glfwGetWindowUserPointer(window));
   InputHandler& inputHandler = *(InputHandler*)windowWrapper->getInputHandler();
   inputHandler.onMouseMove(static_cast<int>(xPos), static_cast<int>(yPos));
   //get width and height
   int winHeight;
   int winWidth;

   glfwGetWindowSize(window, &winWidth, &winHeight);

   GLfloat mx = ((2.0f * xPos / winWidth) - 1.0f);
   GLfloat my = (1.0f - (2.0f * yPos / winHeight));
   //((Window*)glfwGetWindowUserPointer(window))->getHandler()->mouseCoords = glm::vec2(mx, my);
   //((Window*)glfwGetWindowUserPointer(window))->getHandler()->setMousePosition(glm::vec2(mx, my));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
   Window* windowWrapper = static_cast<Window*>(glfwGetWindowUserPointer(window));
   InputHandler& inputHandler = *(InputHandler*)windowWrapper->getInputHandler();

   double xpos, ypos;
   glfwGetCursorPos(window, &xpos, &ypos);

   if (button == GLFW_MOUSE_BUTTON_LEFT) {
       if (action == GLFW_PRESS) {
           inputHandler.onMousePress(
               MouseEvent::Button::LEFT
           );
       } else if (action == GLFW_RELEASE) {
           inputHandler.onMouseRelease(
               MouseEvent::Button::LEFT
           );
       }
   } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
       if (action == GLFW_PRESS) {
           inputHandler.onMousePress(
               MouseEvent::Button::RIGHT
           );
       } else if (action == GLFW_RELEASE) {
           inputHandler.onMouseRelease(
               MouseEvent::Button::RIGHT
           );
       }
   }

   /**
   The mods parameter is a combination of one or more of the following constants:

   GLFW_MOD_SHIFT: Set if one of the Shift keys is held down.
   GLFW_MOD_CONTROL: Set if one of the Control keys is held down.
   GLFW_MOD_ALT: Set if one of the Alt keys is held down.
   GLFW_MOD_SUPER: Set if one of the Super keys is held down.
    */
}

void mouseWheelCallback(GLFWwindow* window, double xOffset, double yOffset) {
   Window* windowWrapper = static_cast<Window*>(glfwGetWindowUserPointer(window));
   InputHandler& inputHandler = *(InputHandler*)windowWrapper->getInputHandler();

   inputHandler.onMouseWheel(yOffset);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
   Window* windowWrapper = static_cast<Window*>(glfwGetWindowUserPointer(window));
   InputHandler& inputHandler = *(InputHandler*)windowWrapper->getInputHandler();

   if (action == GLFW_PRESS) {
       inputHandler.onKeyPressed(key);
   } else if (action == GLFW_RELEASE) {
       inputHandler.onKeyReleased(key);
   }
   // GLFW_REPEAT
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
   // make sure the viewport matches the new window dimensions; note that width and
   // height will be significantly larger than specified on retina displays.
   glViewport(0, 0, width, height);
}

Window::Window(const std::string& windowLbl, int width, int height) {
   if (!glfwInit()) {
       throw std::runtime_error("glfwInit failed");
   }
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   // TODO be aware of opengl/opengles/vulkan
   //Context profiles are only defined for OpenGL version 3.2 and above
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
   //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
   glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
   // Keep window initially hidden
   glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

   mpGLFWWindow_ = glfwCreateWindow(width, height, windowLbl.c_str(), NULL, NULL);
   if (mpGLFWWindow_ == nullptr) {
       glfwTerminate();
       throw std::runtime_error("Failed to create GLFW window");
   }
   glfwSetWindowUserPointer(mpGLFWWindow_, this);
   glfwMakeContextCurrent(mpGLFWWindow_);
   glfwSwapInterval(1);
   glfwSetKeyCallback(mpGLFWWindow_, keyCallback);
   glfwSetFramebufferSizeCallback(mpGLFWWindow_, framebuffer_size_callback);
   /*
   glfwSetWindowFocusCallback(mpGLFWWindow_, windowFocusCallback);
   glfwSetCursorEnterCallback(window, mouse_enter_callback);
   */

   glfwSetCursorPosCallback(mpGLFWWindow_, mouse_callback);
   glfwSetMouseButtonCallback(mpGLFWWindow_, mouse_button_callback);
   glfwSetScrollCallback(mpGLFWWindow_, mouseWheelCallback);
}

Window::~Window() {
   glfwDestroyWindow(mpGLFWWindow_);
   glfwTerminate();
}

void Window::update(float dt) {
    glfwPollEvents();
}

void Window::render() {
    // Draw on the window
    glfwSwapBuffers(mpGLFWWindow_);
}

void Window::enableDisplay(bool show) {
   glfwWindowHint(GLFW_VISIBLE, (show) ? GLFW_TRUE : GLFW_FALSE);
   if (show) {
       glfwShowWindow(mpGLFWWindow_);
   } else {
       glfwHideWindow(mpGLFWWindow_);
   }
}

GLFWwindow* Window::getGLFWWindow() const {
   return mpGLFWWindow_;
}

bool Window::isRunning() const {
    return !glfwWindowShouldClose(mpGLFWWindow_);
}

void Window::setVSync(const bool enabled) {
    mSwapInterval_ = static_cast<int>(enabled);
    // 0 disables, 1 enables
    glfwSwapInterval(mSwapInterval_);
}

int Window::getGLFWSwapInterval() const {
    return mSwapInterval_;
}

glm::ivec2 Window::getDimensions() const {
    int winWidth = 0;
    int winHeight = 0;
    glfwGetWindowSize(mpGLFWWindow_, &winWidth, &winHeight);
    return {winWidth, winHeight};
}

InputHandler* Window::getInputHandler() {
    return &mInputHandler_;
}