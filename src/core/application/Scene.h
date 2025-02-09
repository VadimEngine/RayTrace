#pragma once

class Scene {
public:
    virtual void render() = 0;

    virtual void renderUI() = 0;
};