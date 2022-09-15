#include "Engine.h"

#include <utility>
#include <stdexcept>
#include "EmptyScene.h"

Engine *Engine::instance = nullptr;

Engine::~Engine()
{
    if (this->state != STATE_OFF)
        this->unload();
}

void Engine::setScene(Scene *scene) {
    if (scene) {
        if (this->current_scene) this->current_scene->suspend(*this);
        this->current_scene = scene;
        this->current_scene->resume(*this);
    }
}

void Engine::load()
{
    if (this->state != STATE_OFF)
        throw std::domain_error("Engine: engine is already loaded");

    auto builder = this->main_app.config();

#if defined(__ANDROID__) || defined(ANDROID)
    builder->window_config.androidApp(this->androidApp);
#endif

    this->window = std::make_unique<Window>(builder->window_config);

    graphics
        .init()
        .viewport(window->getLogicalSize())
        .clear({0.0f, 0.1f, 0.2f, 1.0f});

    spige_init(&this->engine);
    spige_viewport(
        &engine,
        static_cast<int>(this->window->getLogicalSize()[0]),
        static_cast<int>(this->window->getLogicalSize()[1]));

    this->state = STATE_READY;

#if defined(__ANDROID__) || defined(ANDROID)
    this->engine.asset_mgr = this->androidApp->activity->assetManager;
#endif

    if (!this->current_scene) { // TODO manage how to load app once
        this->main_app.init(*this);
    }

    if (!this->current_scene) {
        LOGW("Unable to run engine: No scene available\n");
        this->window->close();
    }
}

void Engine::unload()
{
    if (this->state == STATE_OFF)
        throw std::domain_error("Engine: engine is already off");

    this->state = STATE_OFF;
    this->window.reset();
    spige_destroy(&this->engine);
}

void Engine::render()
{
    /* Rendering scene */
    this->current_scene->update(*this);
    this->current_scene->render(*this);

    /* Swap front and back buffers */
    this->window->swapBuffers();
}

void Engine::resume() {
    this->paused = false;
}

void Engine::pause() {
    this->paused = true;
}

#if defined(__ANDROID__) || defined(ANDROID)

#include <dlfcn.h>

Engine::Engine(SpigeApplication& main_app, android_app* android_app_ptr)
    : main_app(main_app), androidApp(android_app_ptr) {

    this->androidApp->userData = this;
    this->androidApp->onAppCmd = Engine::androidHandleCmd;
    this->androidApp->onInputEvent = Input::androidHandleInput;

    // Prepare to monitor accelerometer
    this->sensorManager = acquireASensorManagerInstance(android_app_ptr);
    this->accelerometerSensor = ASensorManager_getDefaultSensor(this->sensorManager,
         ASENSOR_TYPE_ACCELEROMETER);
    this->sensorEventQueue = ASensorManager_createEventQueue(this->sensorManager,
          this->androidApp->looper, LOOPER_ID_USER, nullptr, nullptr);

    if (this->androidApp->savedState != nullptr) {
        this->savedState = *(Engine::SavedState*)this->androidApp->savedState;
    }

    Engine::instance = this;
}

void Engine::run() {

    Engine::androidSetActivityDecor(this->androidApp);

    // loop waiting for stuff to do.
    for (;;) {

        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident = ALooper_pollAll(this->paused ? -1 : 0, nullptr, &events,
                                        (void**)&source)) >= 0) {

            // Process this event.
            if (source != nullptr) {
                source->process(this->androidApp, source);
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
            if (this->androidApp->destroyRequested != 0) {
                spige_destroy(&this->engine);
                return;
            }
        }

        if (!this->paused) {
            // Drawing is throttled to the screen update rate, so there
            // is no need to do timing here.

            // Render
            this->render();
            this->input.clearStates();
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
            pEngine->androidApp->savedState = malloc(sizeof(Engine::SavedState));
            *((Engine::SavedState*)pEngine->androidApp->savedState) = pEngine->savedState;
            pEngine->androidApp->savedStateSize = sizeof(Engine::SavedState);
            break;

        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (pEngine->androidApp->window != nullptr) {
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

Engine::Engine(SpigeApplication& mainApp, int argc, char *argv[]) :
    data{argc, argv},
    main_app(mainApp),
    window(nullptr),
    input()
{
    Engine::instance = this;
}

void Engine::run()
{
    this->load();
    this->resume();

    if (!this->current_scene) {
        LOGW("Unable to run engine: No scene available\n");
        return;
    }

    this->current_scene->resume(*this);

    while (!this->window->isShouldClose()) {

        // Poll for and process events
        this->input.clearStates();
        glfwPollEvents();

        // Render
        this->render();
    }
}

#endif

EngineConfig& EngineConfig::windowConfig(Window::Config config) {
    this->window_config = std::move(config);
    return *this;
}
