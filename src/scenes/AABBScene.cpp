
// third party
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
// project
#include "AABBScene.h"
#include "core/application/App.h"

AABBScene::AABBScene(App& parentApp)
: Scene(parentApp) {
    mAABBShader_ = mParentApp_.getResources().getResource<ShaderProgram>("AABBShader");

    mAABBList_ = {
        { {0, 0, 0}, {1, 1, 1} },
        { {-1, -1, -1}, {0, 0, 0} },
        { {-0.5, -0.5, -0.5}, {0.5, 0.5, 0.5} },
        { {-2, 0, 0}, {2, 1, 1} },
        { {1, -2, 0}, {2, 2, 1} },
        { {0, 0, -3}, {1, 1, 3} },
        { {0.2, 0.2, 1.5}, {0.8, 0.8, 2} },
        { {-3, -3, -3}, {-1, -1, -1} },
        { {-4, -1, -1}, {4, 1, 1} },
        { {2, 3, 4}, {3, 4, 5} }
    };
    
    // Lines 
    {
        GLfloat cubeVertices[] = {
            0, 0, 0,  // 0
            1, 0, 0,  // 1
            1, 1, 0,  // 2
            0, 1, 0,  // 3
            0, 0, 1,  // 4
            1, 0, 1,  // 5
            1, 1, 1,  // 6
            0, 1, 1   // 7
        };

        GLuint cubeIndices[] = {
            0, 1, 1, 2, 2, 3, 3, 0,  // Bottom
            4, 5, 5, 6, 6, 7, 7, 4,  // Top
            0, 4, 1, 5, 2, 6, 3, 7   // Vertical edges
        };

        GLuint ebo;
        glGenVertexArrays(1, &cubeLineVAO);
        glGenBuffers(1, &cubeLineVBO);
        glGenBuffers(1, &ebo);

        glBindVertexArray(cubeLineVAO);
        // Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeLineVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
    }
    // Solid
    {
        GLfloat cubeVertices[] = {
            // Positions (each corner of the cube)
            0, 0, 0,  // 0
            1, 0, 0,  // 1
            1, 1, 0,  // 2
            0, 1, 0,  // 3
            0, 0, 1,  // 4
            1, 0, 1,  // 5
            1, 1, 1,  // 6
            0, 1, 1   // 7
        };

        GLuint cubeIndices[] = {
            // Each face is made of two triangles (6 indices per face)
            // Back face
            0, 1, 2,  2, 3, 0,
            // Front face
            4, 5, 6,  6, 7, 4,
            // Left face
            0, 3, 7,  7, 4, 0,
            // Right face
            1, 2, 6,  6, 5, 1,
            // Bottom face
            0, 1, 5,  5, 4, 0,
            // Top face
            3, 2, 6,  6, 7, 3
        };

        GLuint ebo;
        glGenVertexArrays(1, &cubeSolidVAO);
        glGenBuffers(1, &cubeSolidVBO);
        glGenBuffers(1, &ebo);

        glBindVertexArray(cubeSolidVAO);
        // Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeSolidVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
    }

    glBindVertexArray(0);

    glGenBuffers(1, &mAABBSSBO_);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mAABBSSBO_);
    glBufferData(GL_SHADER_STORAGE_BUFFER, mAABBList_.size() * sizeof(AABB), mAABBList_.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mAABBSSBO_); // Bind to binding point 0
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}

void AABBScene::render() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    mAABBShader_->bind();
    mAABBShader_->setMat4("view", mCamera_.getViewMatrix());
    mAABBShader_->setMat4("projection", mCamera_.getProjectionMatrix());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mAABBSSBO_);

    if (mRenderMode_ == RenderMode::LINES) {
        // wireframe
        glBindVertexArray(cubeLineVAO);
        glDrawElementsInstanced(GL_LINES, 24, GL_UNSIGNED_INT, 0, mAABBList_.size());
    } else {
        // solid
        glBindVertexArray(cubeSolidVAO);
        glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, mAABBList_.size());
    }

    glBindVertexArray(0);
}

void AABBScene::renderUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
        ImGui::Begin("Menu", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        
        ImGui::Text("AABB Scene");
        ImGui::Text("FPS: %.1f", double(ImGui::GetIO().Framerate));

        ImGui::Separator();
        ImGui::Text("Move camera with WASD, arrow, space, shift keys");
        ImGui::Text("Switch scenes with Tab key");
        ImGui::Separator();
        ImGui::Text("Render Mode");

        if (ImGui::RadioButton("Lines", mRenderMode_ == RenderMode::LINES)) {
            mRenderMode_ = RenderMode::LINES;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Solid", mRenderMode_ == RenderMode::SOLID)) {
            mRenderMode_ = RenderMode::SOLID;
        }

        ImGui::Separator();

        const auto cameraPos = mCamera_.getPosition();
        const auto cameraForward = mCamera_.getForward();
        const auto cameraRotation = mCamera_.getOrientation();

        ImGui::Text("Camera Position: %.1f %.1f %.1f", cameraPos.x, cameraPos.y, cameraPos.z);
        ImGui::Text("Camera Forward: %.1f %.1f %.1f", cameraForward.x, cameraForward.y, cameraForward.z);
        ImGui::Text("Camera Orientation (quat): %.3f %.3f %.3f %.3f", 
            cameraRotation.x, cameraRotation.y, cameraRotation.z, cameraRotation.w
        );
        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void AABBScene::update(const float dt) {
    const InputHandler& inputHandler = *(mParentApp_.getWindow()->getInputHandler());

   if (inputHandler.isKeyPressed(GLFW_KEY_W)) {
       mCamera_.move(mCamera_.getForward(), mCamera_.getMoveSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_S)) {
       mCamera_.move(mCamera_.getForward(), -mCamera_.getMoveSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_A)) {
       mCamera_.move(mCamera_.getRight(), -mCamera_.getMoveSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_D)) {
       mCamera_.move(mCamera_.getRight(), mCamera_.getMoveSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_SPACE)) {
       mCamera_.move({0,1,0}, mCamera_.getMoveSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
       mCamera_.move({0,1,0}, -mCamera_.getMoveSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_LEFT)) {
       mCamera_.rotate(mCamera_.getUp(), mCamera_.getRotationSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_RIGHT)) {
       mCamera_.rotate(mCamera_.getUp(), -mCamera_.getRotationSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_UP)) {
       mCamera_.rotate(mCamera_.getRight(), mCamera_.getRotationSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_DOWN)) {
       mCamera_.rotate(mCamera_.getRight(), -mCamera_.getRotationSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_Q)) {
       mCamera_.rotate(mCamera_.getForward(), 200.0f * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_E)) {
       mCamera_.rotate(mCamera_.getForward(), -200.0f * dt);
   }
   // Speed
   if (inputHandler.isKeyPressed(GLFW_KEY_PERIOD)) {
       mCamera_.zoom(-mCamera_.getZoomSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_COMMA)) {
       mCamera_.zoom(mCamera_.getZoomSpeed() * dt);
   }
}

void AABBScene::onKeyPress(unsigned int code) {}

void AABBScene::onKeyRelease(unsigned int code) {}

void AABBScene::onMousePress(const MouseEvent& mouseEvent) {}

void AABBScene::onMouseRelease(const MouseEvent& mouseEvent) {}

void AABBScene::onMouseWheel(const MouseEvent& mouseEvent) {}