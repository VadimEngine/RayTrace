
#pragma once
// third party
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// project
#include "core/application/Scene.h"
#include "core/application/Resources.h"
#include "core/graphics/Camera.h"

class AABBScene : public Scene {
public:

    enum class RenderMode {
        LINES, SOLID
    };

    struct AABB {
        float min[3];
        float max[3];
    };

    AABBScene(App& parentApp);

    void render() override;

    void renderUI() override;

    void update(const float dt) override;

    void onKeyPress(unsigned int code) override;

    void onKeyRelease(unsigned int code) override;

    void onMousePress(const MouseEvent& mouseEvent) override;

    void onMouseRelease(const MouseEvent& mouseEvent) override;

    void onMouseWheel(const MouseEvent& mouseEvent) override;

private:
    ShaderProgram* mAABBShader_ = nullptr;
    
    Camera mCamera_;

    unsigned int cubeLineVAO, cubeLineVBO;

    unsigned int cubeSolidVAO, cubeSolidVBO;


    std::vector<AABB> mAABBList_;

    GLuint mAABBSSBO_;

    RenderMode mRenderMode_ = RenderMode::LINES;
};