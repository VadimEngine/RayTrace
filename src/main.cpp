#include "core/application/App.h"

int main() {
    App theApp;
    std::unique_ptr<Window> pWindow = std::make_unique<Window>("ClayEngine Demo", 800, 800);
    theApp.setWindow(std::move(pWindow));
    theApp.initialize();

    theApp.run();

    return 0;
}