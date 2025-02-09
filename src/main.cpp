// standard lib
#include <iostream>
#include <queue>
#include <bitset>
// third party
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// project
#include "core/graphics/Camera.h"
#include "core/graphics/Model.h"
#include "core/application/Resources.h"
#include "core/graphics/ShaderProgram.h"
#include "scenes/StencilScene.h"
#include "scenes/RayTraceScene.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int currentScene = 0;

// input handling

class KeyEvent {
public:
    enum class Type {
        PRESS, RELEASE
    };
private:
    Type mType_;
    unsigned int mCode_;
public:
    KeyEvent(Type type, unsigned int code)
        : mType_(type), mCode_(code) {}

    Type getType() const {
        return mType_;
    }
    unsigned int getCode() const {
        return mCode_;
    }
};


static constexpr unsigned int kMaxQueueSize = 16;
static constexpr unsigned int kKeys = 512;


std::queue<KeyEvent> mKeyEventQueue_;
std::bitset<kKeys> mKeyStates_;


void trimBuffer(std::queue<KeyEvent> eventQueue) {
    while (eventQueue.size() > kMaxQueueSize) {
        eventQueue.pop();
    }
}

// end input handling

void onKeyPressed(int keyCode) {
    mKeyStates_[keyCode] = true;
    mKeyEventQueue_.push(KeyEvent(KeyEvent::Type::PRESS, keyCode));
    trimBuffer(mKeyEventQueue_);
}

void onKeyReleased(int keyCode) {
    mKeyStates_[keyCode] = false;
    mKeyEventQueue_.push(KeyEvent(KeyEvent::Type::RELEASE, keyCode));
    trimBuffer(mKeyEventQueue_);
}

std::optional<KeyEvent> getKeyEvent() {
    if (mKeyEventQueue_.size() > 0) {
        KeyEvent e = mKeyEventQueue_.front();
        mKeyEventQueue_.pop();
        return e;
    } else {
        return std::nullopt;
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
   if (action == GLFW_PRESS) {
       onKeyPressed(key);
   } else if (action == GLFW_RELEASE) {
       onKeyReleased(key);
   }
   // GLFW_REPEAT
}

void loadResources(Resources& resources) {
    resources.loadResource<ShaderProgram>(
        {{ std::string(RESOURCE_PATH) + "/Shaders/compute_shader.glsl",ShaderProgram::ShaderCreateInfo::Type::COMPUTE}},
        "BasicCompute"
    );

    resources.loadResource<ShaderProgram>(
        {{ std::string(RESOURCE_PATH) + "/Shaders/ray_trace.glsl",ShaderProgram::ShaderCreateInfo::Type::COMPUTE}},
        "RayTrace"
    );

    resources.loadResource<ShaderProgram>(
        {
            { std::string(RESOURCE_PATH) + "/Shaders/2.stencil_testing.vs", ShaderProgram::ShaderCreateInfo::Type::VERTEX},
            { std::string(RESOURCE_PATH) + "/Shaders/2.stencil_testing.fs", ShaderProgram::ShaderCreateInfo::Type::FRAGMENT}
        },
        "StencilShader"
    );

    resources.loadResource<ShaderProgram>(
        {
            { std::string(RESOURCE_PATH) + "/Shaders/2.stencil_testing.vs", ShaderProgram::ShaderCreateInfo::Type::VERTEX},
            { std::string(RESOURCE_PATH) + "/Shaders/2.stencil_single_color.fs", ShaderProgram::ShaderCreateInfo::Type::FRAGMENT}
        },
        "StencilShaderSingleColor"
    );

    resources.loadResource<ShaderProgram>(
        {
            { std::string(RESOURCE_PATH) + "/Shaders/quad.vs", ShaderProgram::ShaderCreateInfo::Type::VERTEX},
            { std::string(RESOURCE_PATH) + "/Shaders/quad.fs", ShaderProgram::ShaderCreateInfo::Type::FRAGMENT}
        },
        "Quad"
    );

    resources.loadResource<Texture>({{ std::string(RESOURCE_PATH) + "/Textures/marble.jpg"}}, "Cube");
    resources.loadResource<Texture>({{  std::string(RESOURCE_PATH) + "/Textures/metal.png"}}, "Floor");

    resources.loadResource<ShaderProgram>(
        {{ std::string(RESOURCE_PATH) + "/Shaders/ray_trace_multi.glsl",ShaderProgram::ShaderCreateInfo::Type::COMPUTE}},
        "RayTraceMulti"
    );
}

int main() {
    // glfw: initialize and configure
    if (!glfwInit()) {
        throw std::runtime_error("glfwInit failed");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSwapInterval(1); // vsync


    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    glewExperimental = true;

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        throw std::runtime_error("GLEW Init error");
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    Resources resources;
    loadResources(resources);

    // set up imgui
    {
        const char* glsl_version = "#version 330";

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        //disable .ini file generations
        io.IniFilename = nullptr;

        // Setup Platform/Renderer backends
        if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) { // TODO CHECK PLATFORM
            std::cout << "ImGui_ImplGlfw_InitForOpenGL failed" << std::endl;
            throw std::runtime_error("ImGui_ImplGlfw_InitForOpenGL error");
        }
        if (!ImGui_ImplOpenGL3_Init(glsl_version)) {
            std::cout << "ImGui_ImplOpenGL3_Init failed" << std::endl;
            throw std::runtime_error("ImGui_ImplOpenGL3_Init error");
        }
    }

    // Set scenes
    std::vector<Scene*> scenes = {
        new StencilScene(resources, camera),
        new RayTraceScene(resources, camera)
    };

    // render loop
    while (!glfwWindowShouldClose(window)) {

        processInput(window);

        // Render
        scenes[currentScene]->render();

        // Draw GUI last
        scenes[currentScene]->renderUI();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    while (const auto keyEvent = getKeyEvent()) {
        if (keyEvent.value().getCode() == GLFW_KEY_TAB && keyEvent.value().getType() == KeyEvent::Type::RELEASE) {
            currentScene = (currentScene + 1) % 2;
            std::cout << "Current scene: " << currentScene << std::endl;
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}