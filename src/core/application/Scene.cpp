#include "core/application/App.h"

Scene::Scene(App& parentApp) 
    : mParentApp_ (parentApp) {}


void Scene::onKeyPress(unsigned int code) {}

void Scene::onKeyRelease(unsigned int code) {}

void Scene::onMousePress(const MouseEvent& mouseEvent) {}

void Scene::onMouseRelease(const MouseEvent& mouseEvent) {}

void Scene::onMouseWheel(const MouseEvent& mouseEvent) {}