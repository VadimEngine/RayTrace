#pragma once
// standard lib
#include <chrono>
#include "core/gui/Window.h"
#include "core/application/Resources.h"
#include "core/application/Scene.h"
#include "core/graphics/Camera.h"

class App {
public:
    /** Constructor */
    App();

    /** Destructor */
    ~App();

    void initialize();

    /** Start and do the run loop */
    void run();

    /** Update the application components */
    void update();

    /** Render the application components */
    void render();

    /** If the application is currently running */
    bool isRunning() const;

    /**
     * Quits the application. Quit should be handled by the Application and not the window to ensure
     * proper closure of the application
     */
    void quit();

    /**
     * Set Anti-Aliasing sample size. If the size is 0 then anti aliasing is disabled
     * @param sampleSize Anti aliasing sample size
     */
    void setAntiAliasing(unsigned int sampleSize);

    /** Get Application Window */
    Window* getWindow();

    void setWindow(std::unique_ptr<Window> pWindow);

    /** Get application resources */
    Resources& getResources();

private:
    /** Load/Build the common resources for the scenes in this application */
    void loadResources();

    /** Time of last update call */
    std::chrono::steady_clock::time_point mLastTime_;
    /** The window for this application*/
    std::unique_ptr<Window> mpWindow_;

    /** Resource for this application that can be shared with child scenes */
    Resources mResources_;

    int mCurrentSceneIdx_ = 0;

    Camera camera;

    std::vector<std::unique_ptr<Scene>> mScenes_;
};