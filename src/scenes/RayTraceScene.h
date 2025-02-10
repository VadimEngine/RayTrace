#pragma once
// project
#include "core/application/Scene.h"
#include "core/application/Resources.h"
#include "core/graphics/Camera.h"

// TODO see if you can use the depth buffer to only draw if nearer than other renders

class RayTraceScene : public Scene {
public:
    RayTraceScene(App& parentAppa);

    void render() override;

    void renderUI() override;

    void update(const float dt) override;

    void onKeyPress(unsigned int code) override;

    void onKeyRelease(unsigned int code) override;

    void onMousePress(const MouseEvent& mouseEvent) override;

    void onMouseRelease(const MouseEvent& mouseEvent) override;

    void onMouseWheel(const MouseEvent& mouseEvent) override;

private:

    ShaderProgram* mpBasicCompute_ = nullptr;
    ShaderProgram* mpRayTraceCompute_ = nullptr;
    ShaderProgram* mpQuadShader_ = nullptr;


    GLuint quadVAO, quadVBO;

    GLuint framebuffer, texture;


    const int dataSize = 1024; // Array of 1024 integers
    std::vector<int> data;

    Camera mCamera_;

};