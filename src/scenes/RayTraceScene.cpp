// standard lib
#include <iostream>
// third party
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
// project
#include "scenes/RayTraceScene.h"
#include "core/application/App.h"


RayTraceScene::RayTraceScene(App& parentApp) 
    : Scene(parentApp), mScreenSize_(mParentApp_.getWindow()->getDimensions()) {

    mpBasicCompute_ = mParentApp_.getResources().getResource<ShaderProgram>("BasicCompute");
    mpRayTraceCompute_ = mParentApp_.getResources().getResource<ShaderProgram>("RayTraceMulti");
    mpQuadShader_ = mParentApp_.getResources().getResource<ShaderProgram>("Quad");

    // quad (ccw)
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,   0.0f, 1.0f,  // top-left
        1.0f, -1.0f,   1.0f, 0.0f,  // bottom-right
        -1.0f, -1.0f,   0.0f, 0.0f,  // bottom-left

        -1.0f,  1.0f,   0.0f, 1.0f,  // top-left
        1.0f,  1.0f,   1.0f, 1.0f,  // top-right
        1.0f, -1.0f,   1.0f, 0.0f   // bottom-right
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    // Create Shader Storage Buffer Object (SSBO)
    data.resize(dataSize, 0);

    {
        mpBasicCompute_->bind();

        GLuint ssbo;
        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, dataSize * sizeof(int), data.data(), GL_DYNAMIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

        // Dispatch Compute Shader
        glDispatchCompute(dataSize / 256, 1, 1); // 1024 elements, 256 per workgroup → 4 workgroups

        // Ensure execution finishes
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // Retrieve the modified data from GPU
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, dataSize * sizeof(int), data.data());

        // Print some results
        std::cout << "First 10 elements after compute shader:\n";
        for (int i = 0; i < 10; ++i) {
            std::cout << data[i] << " ";
        }
        std::cout << std::endl;
    }

    {
        struct Sphere {
            glm::vec3 center;
            float radius;
            glm::vec3 color;
            float reflectivity;
        };

        //rayTraceMultiProgram.bind();
        mpRayTraceCompute_->bind();

        // Define a list of spheres
        std::vector<Sphere> spheres = {
            {{ 0.0f,  0.0f,  -5.0f}, 1.0f, {1.0f, 0.2f, 0.2f}, 0.5f},  // Red, 50% reflective
            {{5.5f,  1.5f,  8.0f}, 0.7f, {0.2f, 1.0f, 0.2f}, 0.2f},  // Green, 20% reflective
            {{ 1.5f,  1.5f,  7.5f}, 1.2f, {0.2f, 0.2f, 1.0f}, 0.7f}   // Blue, 70% reflective
        };

        GLuint ssbo;
        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, spheres.size() * sizeof(Sphere), spheres.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo); // Bind to binding=1
        mpRayTraceCompute_->setInt("numSpheres", spheres.size());
    }

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Create the texture (2D texture for color attachment)
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mScreenSize_.x, mScreenSize_.y, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);

    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
    }
}

void RayTraceScene::render() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glm::mat4 view = mCamera_.getViewMatrix();
    glm::mat4 projection = mCamera_.getProjectionMatrix();

    glClearColor(.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //glUseProgram(computeRay);
    mpRayTraceCompute_->bind();
    mpRayTraceCompute_->setMat4("viewMatrix", view); // TOD REMOVE since we only need inverse
    mpRayTraceCompute_->setMat4("projMatrix", projection);
    mpRayTraceCompute_->setMat4("invProjMatrix", glm::inverse(projection));
    mpRayTraceCompute_->setMat4("invViewMatrix", glm::inverse(view));
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glDispatchCompute((mScreenSize_.x + 15) / 16, (mScreenSize_.y + 15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);  // Ensure updates are visible

    glBindTexture(GL_TEXTURE_2D, texture);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    mpQuadShader_->bind();
    glBindVertexArray(quadVAO);
    glBindTexture(GL_TEXTURE_2D, texture);
    //quad.setMat4("projection", projection);
    mpQuadShader_->setInt("screenTexture", 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RayTraceScene::renderUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
        ImGui::Begin("Menu", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

        ImGui::Text("RayTrace Scene");
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

void RayTraceScene::update(const float dt) {
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

void RayTraceScene::onKeyPress(unsigned int code) {}

void RayTraceScene::onKeyRelease(unsigned int code) {}

void RayTraceScene::onMousePress(const MouseEvent& mouseEvent) {}

void RayTraceScene::onMouseRelease(const MouseEvent& mouseEvent) {}

void RayTraceScene::onMouseWheel(const MouseEvent& mouseEvent) {}