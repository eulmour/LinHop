#ifndef SPIGE_ENGINE_H
#define SPIGE_ENGINE_H

#include <memory>
#include <string>

#include "Framework.h"
#include "Window.h"
#include "Graphics.h"
#include "Input.h"

#if defined(__ANDROID__) || defined(ANDROID)
#include <android/sensor.h>
#include <EGL/egl.h>
#else
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#endif

class Engine;

class Scene {

public:
    Scene() = default;
    explicit Scene(Engine&) {};
    virtual ~Scene() = default;

    virtual void resume(Engine&) = 0;
    virtual void suspend(Engine&) = 0;
    virtual void update(Engine&) = 0;
    virtual void render(Engine&) = 0;
};

class EngineConfig {

public:
    EngineConfig& windowConfig(Window::Config windowConfig);

    Window::Config m_WindowConfig;
};

class SpigeApplication {
public:
    virtual ~SpigeApplication() = default;
    virtual void init(Engine& e) = 0;
    virtual std::unique_ptr<EngineConfig> config() = 0;
};

class Engine {

public:
    ~Engine();
    void load();
    void unload();
    void run();
    void resume();
    void pause();
    void render();

    Scene* getScene() { return this->currentScene; }
    void setScene(Scene* scene);

    std::unique_ptr<Window> window;
    Graphics graphics;
    Input input;

    static Engine* instance;

private:
    bool paused{true};
    spige engine{};
    enum state state{STATE_OFF};
    SpigeApplication& mainApp;
    Scene* currentScene{nullptr};

#if defined(__ANDROID__) || defined(ANDROID)
    public:
        Engine(SpigeApplication& mainApp, android_app* androidApp);

        android_app* androidApp;

        ASensorManager*     sensorManager{nullptr};
        const ASensor*      accelerometerSensor{nullptr};
        ASensorEventQueue*  sensorEventQueue{nullptr};
        struct SavedState {} savedState{};

    private:
        static void androidHandleCmd(android_app* app, int32_t cmd);
        static ASensorManager* acquireASensorManagerInstance(android_app* app);
        static void androidSetActivityDecor(struct android_app* app);
#else
public:
    Engine(SpigeApplication& mainApp, int argc, char* argv[]);

    struct CmdArgs {
        int argc;
        char** argv;
    } data;

#endif
};

#endif //SPIGE_ENGINE_H
