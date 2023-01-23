#include "Engine.h"

#include <iostream>
#include <utility>
#include <stdexcept>
#include <exception>
#include "File.h"
#include "EmptyScene.h"
#include "LogActivity.h"
#ifdef __EMSCRIPTEN__
#   include <emscripten/emscripten.h>
#endif

namespace wuh {

Engine::~Engine()
{
    if (state_ != STATE_OFF)
        this->unload();
}

void Engine::push_scene(std::unique_ptr<Scene> scene) {

    if (!!scene) {
        if (scene_.size() != 0) {
            scene_.top()->suspend(*this);
            this->log() << scene_.top()->title() << " suspended\n";
        }
        scene_.push(std::move(scene));
        if (!paused_) {
            scene_.top()->resume(*this);
            this->log() << scene_.top()->title() << " resumed\n";
        }
    }
}

void Engine::pop_scene() {
    if (scene_.size() < 1) {
        return;
    }

	scene_.top()->suspend(*this);
	this->log() << scene_.top()->title() << " suspended\n";
	scene_.pop();

    if (scene_.size() > 0) {
		scene_.top()->resume(*this);
		this->log() << scene_.top()->title() << " resumed\n";
    }
}

void Engine::load()
{
    if (state_ != STATE_OFF)
        throw std::runtime_error("Engine: engine is already loaded");

    auto builder = main_app_.config();

#if defined(__ANDROID__) || defined(ANDROID)
    builder->window_config.android_app(this->android_app);
#endif

    builder->window_config.user_pointer(reinterpret_cast<void*>(this));
    this->window = std::make_unique<Window>(builder->window_config);

#ifdef __EMSCRIPTEN__
    int w = EM_ASM_INT( return window.innerWidth; );
    int h = EM_ASM_INT( return window.innerHeight; );
    window->size({w, h});
#endif

    graphics
        .init()
        .size(window->size())
        .viewport(window->physical_size())
        .clear({0.0f, 0.1f, 0.2f, 1.0f});

    // Check openGL on the system
    int opengl_info[] = { GL_VENDOR, GL_RENDERER, GL_VERSION /*, GL_EXTENSIONS */ };

    for (int value = 0; value != sizeof(opengl_info) / sizeof(int); value++) {
        const GLubyte* temp_str = glGetString(opengl_info[value]);
        LOGI("OpenGL Info: %s", temp_str);
        this->log() << "OpenGL Info: " << temp_str << "\n\n";
    }

    try {
        std::string current_dir = File::cwd();
        LOGI("Current working directory: %s", current_dir.c_str());
        this->log() << "Current working directory: " << current_dir.c_str() << "\n\n";
    } catch (const std::exception& exception) {
        LOGW("Failed to get current working directory: %s", strerror(errno));
    }

    state_ = STATE_READY;

#if defined(__ANDROID__) || defined(ANDROID)
    this->engine.asset_mgr = this->android_app->activity->assetManager;
#endif

    if (scene_.size() == 0) { // TODO manage how to load app once
        main_app_.init(*this);
    }

    if (scene_.size() == 0) {
        LOGW("Unable to run engine: No scene available");
        this->window->close();
    }
}

void Engine::unload()
{
    if (state_ == STATE_OFF)
        throw std::runtime_error("Engine: engine is already off");

    state_ = STATE_OFF;
    this->window.reset();
}

void Engine::render()
{
    /* Rendering scene */
    scene_.top()->render(*this);

    /* Swap front and back buffers */
    this->window->swap_buffers();
}

void Engine::show_log()
{
    auto log_screen = std::unique_ptr<Scene>(reinterpret_cast<Scene*>(new LogActivity(*this, log_stream_.str())));
    this->push_scene(std::move(log_screen));
}

void Engine::resume() {
    paused_ = false;
}

void Engine::pause() {
    paused_ = true;
}

#if defined(__ANDROID__) || defined(ANDROID)

#include <dlfcn.h>

Engine::Engine(Game& main_app, android_app* android_app_ptr)
    : main_app_(main_app), android_app(android_app_ptr) {

    this->android_app->userData = this;
    this->android_app->onAppCmd = Engine::androidHandleCmd;
    this->android_app->onInputEvent = Input::android_handle_input;

    // Prepare to monitor accelerometer
    this->sensorManager = acquireASensorManagerInstance(android_app_ptr);
    this->accelerometerSensor = ASensorManager_getDefaultSensor(this->sensorManager,
         ASENSOR_TYPE_ACCELEROMETER);
    this->sensorEventQueue = ASensorManager_createEventQueue(this->sensorManager,
          this->android_app->looper, LOOPER_ID_USER, nullptr, nullptr);

    if (this->android_app->savedState != nullptr) {
        this->savedState = *(Engine::SavedState*)this->android_app->savedState;
    }
}

void Engine::run() {

    Engine::androidSetActivityDecor(this->android_app);

    // loop waiting for stuff to do.
    for (;;) {

        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident = ALooper_pollAll(paused_ ? -1 : 0, nullptr, &events,
                                        (void**)&source)) >= 0) {

            // Process this event.
            if (source != nullptr) {
                source->process(this->android_app, source);
            }

            // If a sensor has data, process it now.
            if (ident == LOOPER_ID_USER) {
                if (this->accelerometerSensor != nullptr) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(this->sensorEventQueue,
                                                       &event, 1) > 0) {
                        this->engine.acceleration = event.acceleration;
                    }
                }
            }

            // Check if we are exiting.
            if (this->android_app->destroyRequested != 0) {
                engine_destroy(&this->engine);
                return;
            }
        }

        if (!paused_) {
            // Drawing is throttled to the screen update rate, so there
            // is no need to do timing here.

            // Render
            this->render();
            this->input.clear_states();
        }
    }
}

/*
* AcquireASensorManagerInstance(void)
*    Workaround ASensorManager_getInstance() deprecation false alarm
*    for Android-N and before, when compiling with NDK-r15
*/
ASensorManager* Engine::acquireASensorManagerInstance(android_app* app) {

    typedef ASensorManager *(*PF_GETINSTANCEFORPACKAGE)(const char *name);
    void* androidHandle = dlopen("libandroid.so", RTLD_NOW);

    auto getInstanceForPackageFunc = (PF_GETINSTANCEFORPACKAGE)
        dlsym(androidHandle, "ASensorManager_getInstanceForPackage");

    if (getInstanceForPackageFunc) {
        JNIEnv* env = nullptr;
        app->activity->vm->AttachCurrentThread(&env, nullptr);

        jclass android_content_Context = env->GetObjectClass(app->activity->clazz);

        jmethodID midGetPackageName = env->GetMethodID(android_content_Context,
                                                       "getPackageName",
                                                       "()Ljava/lang/String;");

        auto packageName= (jstring)env->CallObjectMethod(app->activity->clazz,
                                                         midGetPackageName);

        const char *nativePackageName = env->GetStringUTFChars(packageName, nullptr);
        ASensorManager* mgr = getInstanceForPackageFunc(nativePackageName);
        env->ReleaseStringUTFChars(packageName, nativePackageName);
        app->activity->vm->DetachCurrentThread();
        if (mgr) {
            dlclose(androidHandle);
            return mgr;
        }
    }

    typedef ASensorManager *(*PF_GETINSTANCE)();
    auto getInstanceFunc = (PF_GETINSTANCE)
        dlsym(androidHandle, "ASensorManager_getInstance");

    // by all means at this point, ASensorManager_getInstance should be available
    if (!getInstanceFunc) {
        LOGE("Unable to get getInstanceFunc");
        abort();
    }

    dlclose(androidHandle);

    return getInstanceFunc();
}

/**
* Process the next main command.
*/
void Engine::androidHandleCmd(struct android_app* app, int32_t cmd) {

    auto* pEngine = (Engine*)app->userData;

    switch (cmd) {

        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state. Do so.
            pEngine->android_app->savedState = malloc(sizeof(Engine::SavedState));
            *((Engine::SavedState*)pEngine->android_app->savedState) = pEngine->savedState;
            pEngine->android_app->savedStateSize = sizeof(Engine::SavedState);
            break;

        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (pEngine->android_app->window != nullptr) {
                pEngine->load();
            }
            break;

        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            pEngine->unload();
            break;

        case APP_CMD_GAINED_FOCUS:
            // When our app gains focus, we start monitoring the accelerometer.
            if (pEngine->accelerometerSensor != nullptr) {
                ASensorEventQueue_enableSensor(pEngine->sensorEventQueue,
                                               pEngine->accelerometerSensor);
                // We'd like to get 60 events per second (in microseconds).
                ASensorEventQueue_setEventRate(pEngine->sensorEventQueue,
                                               pEngine->accelerometerSensor, (1000L / 60) * 1000);
            }

            pEngine->current_scene->resume(*pEngine);
            pEngine->resume();
            pEngine->state = STATE_BUSY;
            break;

        case APP_CMD_LOST_FOCUS:
            // When our app loses focus, we stop monitoring the accelerometer.
            // This is to avoid consuming battery while not being used.
            if (pEngine->accelerometerSensor != nullptr) {
                ASensorEventQueue_disableSensor(pEngine->sensorEventQueue,
                                                pEngine->accelerometerSensor);
            }

            // Also stop animating.
            pEngine->current_scene->suspend(*pEngine);
            pEngine->pause();
            pEngine->state = STATE_READY;
            break;

        case APP_CMD_WINDOW_RESIZED:
            if (pEngine->state == STATE_BUSY) {
                pEngine->current_scene->suspend(*pEngine);
                pEngine->unload();
                pEngine->load();
                pEngine->current_scene->resume(*pEngine);
            }
            break;

        default: return;
    }
}

void Engine::androidSetActivityDecor(struct android_app* app) {

    JNIEnv* env{};
    app->activity->vm->AttachCurrentThread(&env, NULL);

    jclass activityClass = env->FindClass("android/app/NativeActivity");
    jmethodID getWindow = env->GetMethodID(activityClass, "getWindow", "()Landroid/view/Window;");

    jclass windowClass = env->FindClass("android/view/Window");
    jmethodID getDecorView = env->GetMethodID(windowClass, "getDecorView", "()Landroid/view/View;");

    jclass viewClass = env->FindClass("android/view/View");
    jmethodID setSystemUiVisibility = env->GetMethodID(viewClass, "setSystemUiVisibility", "(I)V");

    jobject window = env->CallObjectMethod(app->activity->clazz, getWindow);

    jobject decorView = env->CallObjectMethod(window, getDecorView);

    jfieldID flagFullscreenID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_FULLSCREEN", "I");
    jfieldID flagHideNavigationID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_HIDE_NAVIGATION", "I");
    jfieldID flagImmersiveStickyID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_IMMERSIVE_STICKY", "I");

    const int flagFullscreen = env->GetStaticIntField(viewClass, flagFullscreenID);
    const int flagHideNavigation = env->GetStaticIntField(viewClass, flagHideNavigationID);
    const int flagImmersiveSticky = env->GetStaticIntField(viewClass, flagImmersiveStickyID);
    const int flag = flagFullscreen | flagHideNavigation | flagImmersiveSticky;

    env->CallVoidMethod(decorView, setSystemUiVisibility, flag);

    app->activity->vm->DetachCurrentThread();
}

#else

Engine::Engine(Game& mainApp, int argc, char *argv[])
    : window(nullptr)
    , input()
    , main_app_(mainApp)
    , data{argc, argv}
{}

static std::function<void()> registered_loop;
static void loop_iteration() {
	registered_loop();
}

void Engine::run() {

    try {
        this->load();
    } catch(const std::exception& exception) {
        LOGE_PRINT("%s", exception.what());
        return;
    }

    this->resume();

    if (scene_.size() == 0) {
        LOGW("Unable to run engine: No scene available");
        scene_.push(std::make_unique<EmptyScene>());
    }

    scene_.top()->resume(*this);
    this->log() << scene_.top()->title() << " resumed\n";

    registered_loop = [&]() {
        // Poll for and process events
        glfwPollEvents();

        // Render
        this->render();

        this->input.clear_states();
    };

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(loop_iteration, 0, 1);
#else
    while (!this->window->should_close()) {
        loop_iteration();
    }
#endif
    glfwTerminate();
}

#endif

EngineConfig& EngineConfig::window(Window::Config config) {
    this->window_config = std::move(config);
    return *this;
}

} // end of namespace wuh