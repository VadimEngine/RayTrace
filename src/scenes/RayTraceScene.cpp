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


namespace {
    static const unsigned int SCR_WIDTH = 800;
    static const unsigned int SCR_HEIGHT = 600;
}

RayTraceScene::RayTraceScene(Resources& resources, Camera& camera) 
: camera(camera) {

    mpBasicCompute_ = resources.getResource<ShaderProgram>("BasicCompute");
    mpRayTraceCompute_ = resources.getResource<ShaderProgram>("RayTraceMulti");
    mpQuadShader_ = resources.getResource<ShaderProgram>("Quad");

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
        resources.getResource<ShaderProgram>("BasicCompute")->bind();

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
        resources.getResource<ShaderProgram>("RayTraceMulti")->bind();

        // Define a list of spheres
        std::vector<Sphere> spheres = {
            {{ 5.0f,  1.0f,  6.0f}, 1.0f, {1.0f, 0.2f, 0.2f}, 0.5f},  // Red, 50% reflective
            {{5.5f,  1.5f,  8.0f}, 0.7f, {0.2f, 1.0f, 0.2f}, 0.2f},  // Green, 20% reflective
            {{ 1.5f,  1.5f,  10.5f}, 1.2f, {0.2f, 0.2f, 1.0f}, 0.7f}   // Blue, 70% reflective
        };

        GLuint ssbo;
        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, spheres.size() * sizeof(Sphere), spheres.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo); // Bind to binding=1
        resources.getResource<ShaderProgram>("RayTraceMulti")->setInt("numSpheres", spheres.size());
    }

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Create the texture (2D texture for color attachment)
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
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
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);


    glClearColor(.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //glUseProgram(computeRay);
    mpRayTraceCompute_->bind();
    mpRayTraceCompute_->setMat4("viewMatrix", view); // TOD REMOVE since we only need inverse
    mpRayTraceCompute_->setMat4("projMatrix", projection);
    mpRayTraceCompute_->setMat4("invProjMatrix", glm::inverse(projection));
    mpRayTraceCompute_->setMat4("invViewMatrix", glm::inverse(view));
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glDispatchCompute((SCR_WIDTH + 15) / 16, (SCR_HEIGHT + 15) / 16, 1);
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
    ImGui_ImplGlfw_NewFrame(); // TODO check platform
    ImGui::NewFrame();
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(250, 480), ImGuiCond_FirstUseEver);
        ImGui::Begin("Menu", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        
        ImGui::Text("RayTrace Scene");
        ImGui::Text("FPS: %.1f", double(ImGui::GetIO().Framerate));

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // TODO check platform
}
