#pragma once
// third party
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// project
#include "core/application/Scene.h"
#include "core/application/Resources.h"
#include "core/graphics/Camera.h"

class StencilScene : public Scene {
public:
    StencilScene(App& parentApp);

    void render() override;

    void renderUI() override;

    void update(const float dt) override;

    void onKeyPress(unsigned int code) override;

    void onKeyRelease(unsigned int code) override;

    void onMousePress(const MouseEvent& mouseEvent) override;

    void onMouseRelease(const MouseEvent& mouseEvent) override;

    void onMouseWheel(const MouseEvent& mouseEvent) override;

private:
    ShaderProgram* mpStencilShader_ = nullptr;
    
    ShaderProgram* mpStencilShaderSingleColor_ = nullptr;

    Texture* mpFloorTexture = nullptr;

    Texture* mpCubeTexture = nullptr;


    Camera mCamera_;

    unsigned int planeVAO, planeVBO;

    unsigned int cubeVAO, cubeVBO;



};