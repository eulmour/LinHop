#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include <memory>
#include <string>
#include <stack>
#include <sstream>

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

namespace wuh {

class Engine;

struct Activity {

    Activity() = default;
    explicit Activity(Engine&) {};
    virtual ~Activity() = default;

    virtual void resume(Engine&) = 0;
    virtual void suspend(Engine&) = 0;
    virtual void render(Engine&) = 0;
    virtual const char* title() { return "Untitled"; }
};

struct EngineConfig {
    EngineConfig& window(Window::Config config);
    Window::Config window_config;
};

struct Game {
    virtual ~Game() = default;
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
    void show_log();
    std::stringstream& log() { return log_stream_; }

    void push_activity(std::unique_ptr<Activity> activity);
    void pop_activity();

    std::unique_ptr<Window> window;
    Graphics graphics;
    Input input;

private:
    bool paused_{ true };
    enum state state_{ STATE_OFF };
    Game& main_app_;
    std::stack<std::unique_ptr<Activity>> activity_;
    std::stringstream log_stream_;

#if defined(__ANDROID__) || defined(ANDROID)
    public:
        Engine(Game& main_app, struct android_app* android_app_ptr);

        struct android_app* android_app;

        ASensorManager*     sensorManager{nullptr};
        const ASensor*      accelerometerSensor{nullptr};
        ASensorEventQueue*  sensorEventQueue{nullptr};
        struct SavedState {} savedState{};
        ASensorVector       acceleration;

    private:
        static void androidHandleCmd(struct android_app* app, int32_t cmd);
        static ASensorManager* acquireASensorManagerInstance(struct android_app* app);
        static void androidSetActivityDecor(struct android_app* app);
#else
public:
    Engine(Game& mainApp, int argc, char* argv[]);

    struct CmdArgs {
        int argc;
        char** argv;
    } data;

#endif
};

} // end of namespace wuh
#endif //ENGINE_ENGINE_H
