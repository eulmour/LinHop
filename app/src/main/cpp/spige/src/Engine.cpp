#include "Engine.h"

#include <utility>
#include "EmptyScene.h"

Engine *Engine::instance = nullptr;

Engine::~Engine()
{
    spige_destroy(&this->engine);
}

void Engine::setScene(Scene *scene) {
    if (scene) {
        this->currentScene->suspend(*this);
        this->currentScene = scene;
        this->currentScene->resume(*this);
    }
}

void Engine::load()
{
    if (this->state != STATE_OFF)
        return;

    this->currentScene = new EmptyScene(); // TODO dangling ptr

    auto builder = this->mainApp.config();

#if defined(__ANDROID__) || defined(ANDROID)
    builder->m_WindowConfig.androidApp(this->androidApp);
#endif

    this->window = std::make_unique<Window>(builder->m_WindowConfig);

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
    this->engine.asset_mgr = this->androidApp->activity->assetManager;
    this->mainApp.init(*this);
}

void Engine::unload()
{
    currentScene->suspend(*this);
}

void Engine::render()
{
    /* Rendering scene */
    this->currentScene->update(*this);
    this->currentScene->render(*this);

    /* Swap front and back buffers */
    this->window->swapBuffers();
}

#if defined(__ANDROID__) || defined(ANDROID)

#include <dlfcn.h>

Engine::Engine(SpigeApplication& mainApp, android_app* androidApp)
    : mainApp(mainApp), androidApp(androidApp) {

    this->androidApp->userData = this;
    this->androidApp->onAppCmd = Engine::androidHandleCmd;
    this->androidApp->onInputEvent = Input::androidHandleInput;

    // Prepare to monitor accelerometer
//    this->sensorManager = ASensorManager_getInstance();
    this->sensorManager = acquireASensorManagerInstance(androidApp);
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
                this->window.reset();
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

//void Engine::render() {
//
//    if (this->display == nullptr) {
//        // No display.
//        return;
//    }
//
//    float* backgroundColor = this->currentScene->getBackgroundColor();
//
//    glClearColor(
//        backgroundColor[0],
//        backgroundColor[1],
//        backgroundColor[2],
//        backgroundColor[3]);
//
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    if (!this->currentScene->render(*this)) {
//        ANativeActivity_finish(this->androidApp->activity);
//    }
//
//}

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

    auto* myApp = (Engine*)app->userData;

    switch (cmd) {

        case APP_CMD_SAVE_STATE:

            // The system has asked us to save our current state.  Do so.
            myApp->androidApp->savedState = malloc(sizeof(Engine::SavedState));
            *((Engine::SavedState*)myApp->androidApp->savedState) = myApp->savedState;
            myApp->androidApp->savedStateSize = sizeof(Engine::SavedState);

            break;

        case APP_CMD_INIT_WINDOW:

            // The window is being shown, get it ready.
            if (myApp->androidApp->window != nullptr) {
                myApp->load();
                myApp->resume();

                if (!myApp->currentScene) {
                    LOGW("Unable to run engine: No scene available\n");
                }

                myApp->currentScene->resume(*myApp);
            }

            break;

        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            myApp->window.reset();

            break;

        case APP_CMD_GAINED_FOCUS:
            // When our app gains focus, we start monitoring the accelerometer.
            if (myApp->accelerometerSensor != nullptr) {
                ASensorEventQueue_enableSensor(myApp->sensorEventQueue,
                                               myApp->accelerometerSensor);
                // We'd like to get 60 events per second (in microseconds).
                ASensorEventQueue_setEventRate(myApp->sensorEventQueue,
                                               myApp->accelerometerSensor, (1000L / 60) * 1000);
            }

            myApp->currentScene->resume(*myApp);
            myApp->resume();

            break;

        case APP_CMD_LOST_FOCUS:
            // When our app loses focus, we stop monitoring the accelerometer.
            // This is to avoid consuming battery while not being used.
            if (myApp->accelerometerSensor != nullptr) {
                ASensorEventQueue_disableSensor(myApp->sensorEventQueue,
                                                myApp->accelerometerSensor);
            }

            // Also stop animating.
            myApp->currentScene->suspend(*myApp);
            myApp->pause();

            break;

        default: return;
    }
}

#else

Engine::Engine(SpigeApplication& mainApp, int argc, char *argv[]) :
    data{argc, argv},
    mainApp(mainApp),
    window(nullptr),
    input()
{
    Engine::instance = this;
}

void Engine::run()
{
    this->load();
    this->resume();

    if (!this->currentScene) {
        LOGW("Unable to run engine: No scene available\n");
        return;
    }

    this->currentScene->resume(*this);

    while (!this->window->isShouldClose()) {

        // Poll for and process events
        this->input.clearStates();
        glfwPollEvents();

        // Render
        this->render();
    }
}

#endif

EngineConfig& EngineConfig::windowConfig(Window::Config windowConfig) {
    this->m_WindowConfig = std::move(windowConfig);
    return *this;
}
