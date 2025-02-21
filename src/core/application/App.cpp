// standard lib
#include <iostream>
// third party
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
// project
#include "App.h"
#include "scenes/StencilScene.h"
#include "scenes/RayTraceScene.h"
#include "scenes/AABBScene.h"

App::App() {}

App::~App() {}

void App::initialize() {
    // set up opengl
    glewExperimental = true;

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        throw std::runtime_error("GLEW Init error");
    }
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    
    loadResources();

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
        if (!ImGui_ImplGlfw_InitForOpenGL(mpWindow_->getGLFWWindow(), true)) {
            std::cout << "ImGui_ImplGlfw_InitForOpenGL failed" << std::endl;
            throw std::runtime_error("ImGui_ImplGlfw_InitForOpenGL error");
        }
        if (!ImGui_ImplOpenGL3_Init(glsl_version)) {
            std::cout << "ImGui_ImplOpenGL3_Init failed" << std::endl;
            throw std::runtime_error("ImGui_ImplOpenGL3_Init error");
        }
    }

    mScenes_.emplace_back(std::make_unique<StencilScene>(*this));
    mScenes_.emplace_back(std::make_unique<RayTraceScene>(*this));
    mScenes_.emplace_back(std::make_unique<AABBScene>(*this));

    mCurrentSceneIdx_ = 0;
}

void App::run() {
    mpWindow_->enableDisplay(true);
    // Update and render while application is running
    while (isRunning()) {
        update();
        render();
    }
}

void App::update() {
    // Calculate time since last update (in seconds)
    std::chrono::duration<float> dt = (std::chrono::steady_clock::now() - mLastTime_);
    mLastTime_ = std::chrono::steady_clock::now();
    // Update application content
    mpWindow_->update(dt.count());
    // Update list in reverse order and delete any marked for removal

    InputHandler* handler = (InputHandler*)mpWindow_->getInputHandler();

    // Propagate key events to the scenes
    while (const auto keyEvent = handler->getKeyEvent()) {
        // process and pass to scene
        if (keyEvent.value().getType() == KeyEvent::Type::PRESS) {
           mScenes_[mCurrentSceneIdx_]->onKeyPress(keyEvent.value().getCode());
        } else if (keyEvent.value().getType() == KeyEvent::Type::RELEASE) {
            mScenes_[mCurrentSceneIdx_]->onKeyRelease(keyEvent.value().getCode());
            if (keyEvent.value().getCode() == GLFW_KEY_TAB) {
                mCurrentSceneIdx_ = (mCurrentSceneIdx_ + 1) % mScenes_.size();
            } else if (keyEvent.value().getCode() == GLFW_KEY_ESCAPE) {
                glfwSetWindowShouldClose(mpWindow_->getGLFWWindow(), true);        
            }
        }
    }

    // Propagate mouse events to scenes
    while (const auto mouseEvent = handler->getMouseEvent()) {
        if (mouseEvent.value().getType() == MouseEvent::Type::PRESS) {
            mScenes_[mCurrentSceneIdx_]->onMousePress(mouseEvent.value());
        } else if (mouseEvent.value().getType() == MouseEvent::Type::RELEASE) {
            mScenes_[mCurrentSceneIdx_]->onMouseRelease(mouseEvent.value());
        } else if (mouseEvent.value().getType() == MouseEvent::Type::SCROLL_UP ||
            mouseEvent.value().getType() == MouseEvent::Type::SCROLL_DOWN) {
            mScenes_[mCurrentSceneIdx_]->onMouseWheel(mouseEvent.value());
        }
    }

    mScenes_[mCurrentSceneIdx_]->update(dt.count());
}

void App::render() {
    mScenes_[mCurrentSceneIdx_]->render();
    mScenes_[mCurrentSceneIdx_]->renderUI();

    mpWindow_->render();
}

bool App::isRunning() const {
    return mpWindow_->isRunning();
}

void App::quit() {
    // TODO do any saving
    // Set window to close
    glfwSetWindowShouldClose(((Window*)mpWindow_.get())->getGLFWWindow(), true);
}


Window* App::getWindow() {
    return mpWindow_.get();
}

void App::setWindow(std::unique_ptr<Window> pWindow) {
    mpWindow_ = std::move(pWindow);
}

void App::setAntiAliasing(unsigned int sampleSize) {

}

void App::loadResources() {
    mResources_.loadResource<ShaderProgram>(
        {{ std::string(RESOURCE_PATH) + "/Shaders/compute_shader.glsl:COMPUTE"}},
        "BasicCompute"
    );

    mResources_.loadResource<ShaderProgram>(
        {{ std::string(RESOURCE_PATH) + "/Shaders/ray_trace.glsl:COMPUTE"}},
        "RayTrace"
    );

    mResources_.loadResource<ShaderProgram>(
        {
            { std::string(RESOURCE_PATH) + "/Shaders/2.stencil_testing.vs:VERTEX"},
            { std::string(RESOURCE_PATH) + "/Shaders/2.stencil_testing.fs:FRAGMENT"}
        },
        "StencilShader"
    );

    mResources_.loadResource<ShaderProgram>(
        {
            { std::string(RESOURCE_PATH) + "/Shaders/2.stencil_testing.vs:VERTEX"},
            { std::string(RESOURCE_PATH) + "/Shaders/2.stencil_single_color.fs:FRAGMENT"}
        },
        "StencilShaderSingleColor"
    );

    mResources_.loadResource<ShaderProgram>(
        {
            { std::string(RESOURCE_PATH) + "/Shaders/quad.vs:VERTEX"},
            { std::string(RESOURCE_PATH) + "/Shaders/quad.fs:FRAGMENT"}
        },
        "Quad"
    );

    mResources_.loadResource<ShaderProgram>(
        {
            { std::string(RESOURCE_PATH) + "/Shaders/AABB.vs:VERTEX"},
            { std::string(RESOURCE_PATH) + "/Shaders/AABB.fs:FRAGMENT"}
        },
        "AABBShader"
    );

    mResources_.loadResource<Texture>({{ std::string(RESOURCE_PATH) + "/Textures/marble.jpg"}}, "Cube");
    mResources_.loadResource<Texture>({{ std::string(RESOURCE_PATH) + "/Textures/metal.png"}}, "Floor");

    mResources_.loadResource<ShaderProgram>(
        {{ std::string(RESOURCE_PATH) + "/Shaders/ray_trace_multi.glsl:COMPUTE"}},
        "RayTraceMulti"
    );
}

Resources& App::getResources() {
    return mResources_;
}