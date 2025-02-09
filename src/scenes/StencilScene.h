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
    StencilScene(Resources& resources, Camera& camera);

    void render() override;

    void renderUI() override;

private:
    ShaderProgram* mpStencilShader_ = nullptr;
    
    ShaderProgram* mpStencilShaderSingleColor_ = nullptr;

    Texture* mpFloorTexture = nullptr;

    Texture* mpCubeTexture = nullptr;


    Camera& camera;

    unsigned int planeVAO, planeVBO;

    unsigned int cubeVAO, cubeVBO;



};