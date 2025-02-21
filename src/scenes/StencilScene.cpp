// third party
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
// project
#include "StencilScene.h"
#include "core/application/App.h"


StencilScene::StencilScene(App& parentApp)
: Scene(parentApp) {
    mpStencilShader_ = mParentApp_.getResources().getResource<ShaderProgram>("StencilShader");
    mpStencilShaderSingleColor_ = mParentApp_.getResources().getResource<ShaderProgram>("StencilShaderSingleColor");

    mpCubeTexture = mParentApp_.getResources().getResource<Texture>("Cube");
    mpFloorTexture = mParentApp_.getResources().getResource<Texture>("Floor");

    float cubeVertices[] = {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    float planeVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };

    // cube VAO
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    // plane VAO
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    mpStencilShader_->bind();
    mpStencilShader_->setInt("texture1", 0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // TODO in destructor
    // glDeleteVertexArrays(1, &cubeVAO);
    // glDeleteVertexArrays(1, &planeVAO);
    // glDeleteBuffers(1, &cubeVBO);
    // glDeleteBuffers(1, &planeVBO);
}

void StencilScene::render() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // set uniforms
    mpStencilShaderSingleColor_->bind();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = mCamera_.getViewMatrix();
    glm::mat4 projection = mCamera_.getProjectionMatrix();
    mpStencilShaderSingleColor_->setMat4("view", view);
    mpStencilShaderSingleColor_->setMat4("projection", projection);

    mpStencilShader_->bind();
    mpStencilShader_->setMat4("view", view);
    mpStencilShader_->setMat4("projection", projection);

    // draw floor as normal, but don't write the floor to the stencil buffer, we only care about the containers. We set its mask to 0x00 to not write to the stencil buffer.
    glStencilMask(0x00);
    // floor
    glBindVertexArray(planeVAO);
    glBindTexture(GL_TEXTURE_2D, mpFloorTexture->getId());
    mpStencilShader_->setMat4("model", glm::mat4(1.0f));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // 1st. render pass, draw objects as normal, writing to the stencil buffer
    // --------------------------------------------------------------------
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    // cubes
    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mpCubeTexture->getId());
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    mpStencilShader_->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    mpStencilShader_->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
    // Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
    // the objects' size differences, making it look like borders.
    // -----------------------------------------------------------------------------------------------------------------------------
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    mpStencilShaderSingleColor_->bind();
    float scale = 1.1f;
    // cubes
    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_2D, mpCubeTexture->getId());
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    model = glm::scale(model, glm::vec3(scale, scale, scale));
    mpStencilShaderSingleColor_->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(scale, scale, scale));
    mpStencilShaderSingleColor_->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glEnable(GL_DEPTH_TEST);
}

void StencilScene::renderUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
        ImGui::Begin("Menu", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        
        ImGui::Text("Stencil Scene");
        ImGui::Text("FPS: %.1f", double(ImGui::GetIO().Framerate));

        ImGui::Separator();
        ImGui::Text("Move camera with WASD, arrow, space, shift keys");
        ImGui::Text("Switch scenes with Tab key");
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

void StencilScene::update(const float dt) {
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

void StencilScene::onKeyPress(unsigned int code) {}

void StencilScene::onKeyRelease(unsigned int code) {}

void StencilScene::onMousePress(const MouseEvent& mouseEvent) {}

void StencilScene::onMouseRelease(const MouseEvent& mouseEvent) {}

void StencilScene::onMouseWheel(const MouseEvent& mouseEvent) {}