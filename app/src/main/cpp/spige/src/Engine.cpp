#include "Engine.h"
#include "EmptyScene.h"

Engine *Engine::instance = nullptr;

void Engine::setScene(Scene *scene) {
    if (scene) {
        this->currentScene->suspend(*this);
        this->currentScene = scene;
        this->currentScene->resume(*this);
    }
}

#if defined(__ANDROID__) || defined(ANDROID)

#include <dlfcn.h>

Engine::Engine(IApplication* mainApp, android_app *androidApp)
    : androidApp(androidApp), state{0} {

    this->androidApp->userData = this;
    this->androidApp->onAppCmd = Engine::androidHandleCmd;
    this->androidApp->onInputEvent = Engine::androidHandleInput;

    // Prepare to monitor accelerometer
    this->sensorManager = ASensorManager_getInstance();
    this->accelerometerSensor = ASensorManager_getDefaultSensor(this->sensorManager,
         ASENSOR_TYPE_ACCELEROMETER);
    this->sensorEventQueue = ASensorManager_createEventQueue(this->sensorManager,
          this->androidApp->looper, LOOPER_ID_USER, nullptr, nullptr);

    if (this->androidApp->savedState != nullptr) {
        this->state = *(Engine::SavedState*)this->androidApp->savedState;
    }

    this->resume();
}

Engine::~Engine() {}

void Engine::load() {
//    if (!currentScene)
//        currentScene = std::make_unique<Scene>();
    currentScene->initialize();
}

void Engine::unload() {
    currentScene->destroy();
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
                this->terminateGraphics();
                return;
            }
        }

        if (!this->paused) {
            // Drawing is throttled to the screen update rate, so there
            // is no need to do timing here.
            this->render();
        }
    }
}

void Engine::render() {

    if (this->display == nullptr) {
        // No display.
        return;
    }

    float* backgroundColor = this->currentScene->getBackgroundColor();

    glClearColor(
        backgroundColor[0],
        backgroundColor[1],
        backgroundColor[2],
        backgroundColor[3]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!this->currentScene->render(*this)) {
        ANativeActivity_finish(this->androidApp->activity);
    }

    eglSwapBuffers(this->display, this->surface);
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
* Process the next input event.
*/
int32_t Engine::androidHandleInput(struct android_app* app, AInputEvent* event) {

    Engine *pEngine = (Engine *) app->userData;
    int32_t eventType = AInputEvent_getType(event);

    if (eventType == AINPUT_EVENT_TYPE_MOTION) {

        int32_t motionAction = AMotionEvent_getAction(event);
        int32_t motionType = motionAction & AMOTION_EVENT_ACTION_MASK;
        size_t pointerCount = AMotionEvent_getPointerCount(event); // may overflow buffer

        if (pointerCount < 1)
            return 0;

        switch (motionType) {
            case AMOTION_EVENT_ACTION_MOVE:

                for (size_t i = 0; i < pointerCount; i++) {
                    pEngine->engine.cursor[i][0] = AMotionEvent_getX(event, i);
                    pEngine->engine.cursor[i][1] = AMotionEvent_getY(event, i);
                }

                pEngine->currentScene->onEventPointerMove();
                return 1;

            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:

                for (size_t i = 0; i < pointerCount; i++) {
                    pEngine->engine.cursor[i][0] = AMotionEvent_getX(event, i);
                    pEngine->engine.cursor[i][1] = AMotionEvent_getY(event, i);
                }

                pEngine->currentScene->onEventPointerDown();
                return 1;

            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:
                pEngine->currentScene->onEventPointerUp();
                return 1;

            default: break;
        }

        return 0;

    } else if (eventType == AINPUT_EVENT_TYPE_KEY) {

        int32_t keyAction = AKeyEvent_getAction(event);
        int32_t keyCode = AKeyEvent_getKeyCode(event);

        //Meta state holds info regarding whether shift was held, ctrl, alt, etc...
//        int32_t keyMetaState = AKeyEvent_getMetaState(event);

        if (keyCode == AKEYCODE_VOLUME_UP || keyCode == AKEYCODE_VOLUME_DOWN) {
            return 0;
        }

        if (keyAction == AKEY_EVENT_ACTION_DOWN) {

//            int event_type = 0;
//            char event_key_char = 0;

            switch (keyCode) {
                case AKEYCODE_BACK:
                case AKEYCODE_MENU:
                    if (!pEngine->currentScene->onEventBack()) {
                        ANativeActivity_finish(pEngine->androidApp->activity);
                    }

                    break;
                case AKEYCODE_DPAD_LEFT:
                    pEngine->currentScene->onEventLeft();
                    break;
                case AKEYCODE_DPAD_UP:
                    pEngine->currentScene->onEventUp();
                    break;
                case AKEYCODE_DPAD_RIGHT:
                    pEngine->currentScene->onEventRight();
                    break;
                case AKEYCODE_DPAD_DOWN:
                    pEngine->currentScene->onEventDown();
                    break;
                case AKEYCODE_ENTER:
                    pEngine->currentScene->onEventSelect();
                    break;
                default: break;
            }

//            //if(key_meta_state && AMETA_CAPS_LOCK_ON)//Checks for caps lock
//            //if(key_meta_state && AMETA_SHIFT_ON)//Checks for shift
//            event_key_char = eng->jnii->get_key_event_char(keyAction, keyCode, keyMetaState);
//
//            //get key event returns null if backspace, and we want to catch backspace characters
//            if (keyCode == AKEYCODE_DEL) {
//                event_key_char = '\b';
//            }
//
//            //Filtering out unwanted character through this array
//            event_key_char = INPUT_CHAR_FILTER[static_cast<std::size_t>((unsigned char) event_key_char)];
//
//            if (event_key_char) {
//                event_type = INPUT_KEY_KEYBOARD;
//            }
//            if (event_type) {
//                eng->game->handle_key_input(event_type, event_key_char);
//            }
        }

        return 1;
    }

    return 0;
}

/**
* Process the next main command.
*/
void Engine::androidHandleCmd(struct android_app* app, int32_t cmd) {

    Engine* myApp = (Engine*)app->userData;

    switch (cmd) {

        case APP_CMD_SAVE_STATE:

            // The system has asked us to save our current state.  Do so.
            myApp->androidApp->savedState = malloc(sizeof(Engine::SavedState));
            *((Engine::SavedState*)myApp->androidApp->savedState) = myApp->state;
            myApp->androidApp->savedStateSize = sizeof(Engine::SavedState);

            break;

        case APP_CMD_INIT_WINDOW:

            // The window is being shown, get it ready.
            if (myApp->androidApp->window != nullptr) {
                myApp->window.init();
                myApp->load();
            }

            break;

        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.

            myApp->terminateGraphics();

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

Engine::~Engine()
{
    spige_destroy(&this->engine);
}

void Engine::load()
{
    if (this->state != STATE_OFF)
        return;

    this->currentScene = new EmptyScene(); // TODO dangling ptr

    auto builder = this->mainApp.config();

    this->window = std::make_unique<Window>(WindowConfig()
//        .setSizeCallback([this](int width, int height) mutable {
//            this->window->setLogicalSize({width, height});
//            spige_viewport(&Engine::instance->engine, width, height);
//        })
    );

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
    this->mainApp.init(*this);
}

void Engine::unload()
{
    currentScene->suspend(*this);
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

void Engine::render()
{
    /* Rendering scene */
    this->currentScene->update(*this);
    this->currentScene->render(*this);

    /* Swap front and back buffers */
    this->window->swapBuffers();
}

#endif

EngineConfig& EngineConfig::windowTitle(const std::string& newTitle) {
    this->title = newTitle;
    return *this;
}
