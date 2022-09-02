#include "Main.hpp"
#include <dlfcn.h>
//#include <fcntl.h>

Main::Main(android_app *androidApp)
    : androidApp(androidApp), state{0} {

    this->androidApp->userData = this;
    this->androidApp->onAppCmd = Main::androidHandleCmd;
    this->androidApp->onInputEvent = Main::androidHandleInput;

    // Prepare to monitor accelerometer
    this->sensorManager = ASensorManager_getInstance();
    this->accelerometerSensor = ASensorManager_getDefaultSensor(this->sensorManager,
         ASENSOR_TYPE_ACCELEROMETER);
    this->sensorEventQueue = ASensorManager_createEventQueue(this->sensorManager,
          this->androidApp->looper, LOOPER_ID_USER, nullptr, nullptr);

    if (this->androidApp->savedState != nullptr) {
        this->state = *(Main::SavedState*)this->androidApp->savedState;
    }

    this->resume();
}

Main::~Main() {}

void Main::load() {
    if (!mainScene)
        mainScene = std::make_unique<MainScene>();
}

void Main::unload() {
    mainScene->pause();
}

void Main::run() {

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

void Main::render() {

    if (this->display == nullptr) {
        // No display.
        return;
    }

    glClearColor(
        this->mainScene->backgroundColor[0],
        this->mainScene->backgroundColor[1],
        this->mainScene->backgroundColor[2],
        this->mainScene->backgroundColor[3]
    );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!this->mainScene->draw()) {
        ANativeActivity_finish(this->androidApp->activity);
    }

    eglSwapBuffers(this->display, this->surface);
}

/**
* Initialize an EGL context for the current display.
*/
void Main::initGraphics() {
    // initialize OpenGL ES and EGL

    /*
    * Here specify the attributes of the desired configuration.
    * Below, we select an EGLConfig with at least 8 bits per color
    * component compatible with on-screen windows
    */
    const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
    };

    const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };

    EGLint w, h, format;
    EGLint numConfigs;
    EGLConfig config;

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, nullptr, nullptr);

    /* Here, the application chooses the configuration it desires. In this
    * sample, we have a very simplified selection process, where we pick
    * the first EGLConfig that matches our criteria */
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
    * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
    * As soon as we picked a EGLConfig, we can safely reconfigure the
    * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(this->androidApp->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, this->androidApp->window, nullptr);
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGW("Unable to eglMakeCurrent");
        return;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    this->state.angle = 0;

    LOGI("GL Init: %d", GL_VERSION);

    spige_init(&engine);
    spige_viewport(&engine, w, h);
    engine.asset_mgr = this->androidApp->activity->assetManager;
}

void Main::terminateGraphics() {

    spige_destroy(&this->engine);

    if (this->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(this->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (this->context != EGL_NO_CONTEXT) {
            eglDestroyContext(this->display, this->context);
        }
        if (this->surface != EGL_NO_SURFACE) {
            eglDestroySurface(this->display, this->surface);
        }
        eglTerminate(this->display);
    }

    this->display = EGL_NO_DISPLAY;
    this->context = EGL_NO_CONTEXT;
    this->surface = EGL_NO_SURFACE;
}

/*
* AcquireASensorManagerInstance(void)
*    Workaround ASensorManager_getInstance() deprecation false alarm
*    for Android-N and before, when compiling with NDK-r15
*/
ASensorManager* Main::acquireASensorManagerInstance(android_app* app) {

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
int32_t Main::androidHandleInput(struct android_app* app, AInputEvent* event) {

    Main *myApp = (Main *) app->userData;
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
                    myApp->engine.cursor[i][0] = AMotionEvent_getX(event, i);
                    myApp->engine.cursor[i][1] = AMotionEvent_getY(event, i);
                }

                myApp->mainScene->onEventPointerMove();
                return 1;

            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:

                for (size_t i = 0; i < pointerCount; i++) {
                    myApp->engine.cursor[i][0] = AMotionEvent_getX(event, i);
                    myApp->engine.cursor[i][1] = AMotionEvent_getY(event, i);
                }

                myApp->mainScene->onEventPointerDown();
                return 1;

            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:
                myApp->mainScene->onEventPointerUp();
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
                    if (!myApp->mainScene->onEventBack()) {
                        ANativeActivity_finish(myApp->androidApp->activity);
                    }

                    break;
                case AKEYCODE_DPAD_LEFT:
                    myApp->mainScene->onEventLeft();
                    break;
                case AKEYCODE_DPAD_UP:
                    myApp->mainScene->onEventUp();
                    break;
                case AKEYCODE_DPAD_RIGHT:
                    myApp->mainScene->onEventRight();
                    break;
                case AKEYCODE_DPAD_DOWN:
                    myApp->mainScene->onEventDown();
                    break;
                case AKEYCODE_ENTER:
                    myApp->mainScene->onEventSelect();
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
void Main::androidHandleCmd(struct android_app* app, int32_t cmd) {

    Main* myApp = (Main*)app->userData;

    switch (cmd) {

        case APP_CMD_SAVE_STATE:

            // The system has asked us to save our current state.  Do so.
            myApp->androidApp->savedState = malloc(sizeof(Main::SavedState));
            *((Main::SavedState*)myApp->androidApp->savedState) = myApp->state;
            myApp->androidApp->savedStateSize = sizeof(Main::SavedState);

            break;

        case APP_CMD_INIT_WINDOW:

            // The window is being shown, get it ready.
            if (myApp->androidApp->window != nullptr) {
                myApp->initGraphics();
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

            myApp->mainScene->resume();
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
            myApp->mainScene->pause();
            myApp->pause();

            break;

        default: return;
    }
}

/**
* This is the main entry point of a native application that is using
* android_native_app_glue.  It runs in its own thread, with its own
* event loop for receiving input events and doing other things.
*/
void android_main(struct android_app* state) {
    Main main(state);
    main.run();
}
