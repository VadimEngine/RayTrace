#pragma once
#include "core/application/InputHandler.h"

class App;

class Scene {
public:
    Scene(App& parentApp);

    virtual void render() = 0;

    virtual void update(const float dt) = 0;

    virtual void renderUI() = 0;

    virtual void onKeyPress(unsigned int code);

    virtual void onKeyRelease(unsigned int code);

    virtual void onMousePress(const MouseEvent& mouseEvent);

    virtual void onMouseRelease(const MouseEvent& mouseEvent);

    virtual void onMouseWheel(const MouseEvent& mouseEvent);


protected:
    App& mParentApp_;
};