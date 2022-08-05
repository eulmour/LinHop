#ifndef LINHOP_MAIN_HPP
#define LINHOP_MAIN_HPP

#include <android/sensor.h>
#include "spige.h"

#include "MainScene.hpp"

class Main {
public:

    ~Main() = default;

    void load();
    void unload();
    void run();
    void resume() { paused = false; }
    void pause() { paused = true; }
    void render();

    void initGraphics();
    void terminateGraphics();

private:
    bool paused = true;
    spige engine = {0};
    std::unique_ptr<MainScene> mainScene = nullptr;

#if defined(__ANDROID__) || defined(ANDROID)
    public:
        Main(android_app* androidApp);

        android_app* androidApp;

        EGLDisplay          display = EGL_NO_DISPLAY;
        EGLSurface          surface = EGL_NO_SURFACE;
        EGLContext          context = EGL_NO_CONTEXT;
        ASensorManager*     sensorManager = nullptr;
        const ASensor*      accelerometerSensor = nullptr;
        ASensorEventQueue*  sensorEventQueue = nullptr;
        struct SavedState {
            float angle;
            int32_t x, y;
        } state;

    private:
        static void androidHandleCmd(android_app* app, int32_t cmd);
        static int32_t androidHandleInput(android_app* app, AInputEvent* event);
        static ASensorManager* acquireASensorManagerInstance(android_app* app);
#else
public:
    Main(Application& app, int argc, char* argv[]);

    struct CmdArgs {
        int argc;
        char** argv
    } data;
#endif

};

#endif //LINHOP_MAIN_HPP
