#ifndef LINHOP_MAIN_HPP
#define LINHOP_MAIN_HPP

#include <memory>

#include "spige.h"
#include "game/MainScene.hpp"

#if defined(__ANDROID__) || defined(ANDROID)
#include <android/sensor.h>
#include <EGL/egl.h>
#else
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#endif

class Main {
public:

    ~Main();
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
    Main(int argc, char* argv[]);

    static Main* instance;

    GLFWwindow* window;
    int screen_width = 480;
    int screen_height = 800;

    struct CmdArgs {
        int argc;
        char** argv;
    } data;

    private:
        static void _glfwSizeCallback(GLFWwindow* window, int width, int height);
        static void _glfwCursorCallback(GLFWwindow* window, double xpos, double ypos);
        static void _glfwInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void _glfwMouseCallback(GLFWwindow* window, int button, int action, int mods);

#endif

};

#endif //LINHOP_MAIN_HPP
