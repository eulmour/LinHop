#include "Main.hpp"

Main* Main::instance = nullptr;

Main::Main(int argc, char* argv[]): data{argc, argv} {

    Main::instance = this;
    this->initGraphics();
    this->load();
    this->resume();
}

void Main::_glfwSizeCallback(GLFWwindow* window, int width, int height)
{
    Main::instance->screen_width = width;
    Main::instance->screen_height = height;

    glViewport(0, 0, width, height);
}

void Main::_glfwCursorCallback(GLFWwindow* window, double xpos, double ypos)
{
    Main::instance->engine.cursor[0][0] = xpos;
    Main::instance->engine.cursor[0][1] = ypos;
}

void Main::_glfwInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;

	switch (key) {
		case GLFW_KEY_ESCAPE:

			if (!Main::instance->mainScene->onEventBack()) {
                exit(0);
			}

			break;
		case GLFW_KEY_LEFT:
			Main::instance->mainScene->onEventLeft();
			break;
		case GLFW_KEY_UP:
			Main::instance->mainScene->onEventUp();
			break;
		case GLFW_KEY_RIGHT:
			Main::instance->mainScene->onEventRight();
			break;
		case GLFW_KEY_DOWN:
			Main::instance->mainScene->onEventDown();
			break;
		case GLFW_KEY_ENTER:
			Main::instance->mainScene->onEventSelect();
			break;
		default: break;
	}
}

void Main::_glfwMouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (action != GLFW_PRESS)
        return;

    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
		Main::instance->mainScene->onEventPointerUp();
        break;
    }
}

static void GLAPIENTRY errorOccurredGL(
    GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam)
{
    (void)length;
    (void)userParam;

    printf("Message from OpenGL:\nSource: 0x%x\nType: 0x%x\n"
        "Id: 0x%x\nSeverity: 0x%x\n", source, type, id, severity);
    printf("%s\n", message);

    exit(-1);
}

void Main::load() {
    if (!mainScene)
        mainScene = std::make_unique<MainScene>();
}

void Main::unload() {
    mainScene->pause();
}

void Main::run() {

    this->mainScene->resume();

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(this->window)) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        this->render();
    }
}

void Main::render() {

	/* Poll for and process events */
	glfwPollEvents();

    glClearColor(
        this->mainScene->backgroundColor[0],
        this->mainScene->backgroundColor[1],
        this->mainScene->backgroundColor[2],
        this->mainScene->backgroundColor[3]
    );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Rendering scene */
    if (!this->mainScene->draw()) {
        glfwSetWindowShouldClose(this->window, 1);
    }

	/* Swap front and back buffers */
	glfwSwapBuffers(this->window);
}

void Main::initGraphics() {

    /* Initialize the library */
    if (!glfwInit())
        exit(-1);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

#if defined (__linux__)
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);

	// if (glfwPlatformSupported(GLFW_PLATFORM_WAYLAND))
    // else if (glfwPlatformSupported(GLFW_PLATFORM_X11))
    //     glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif

    /* Create a windowed mode window and its OpenGL context */
    this->window = glfwCreateWindow(this->screen_width, this->screen_height, "LinHop", NULL, NULL);

    if (!this->window) {
        glfwTerminate();
        exit(-1);
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(this->window);
    glfwSetFramebufferSizeCallback(this->window, Main::_glfwSizeCallback);
    glfwSwapInterval(1);
    glfwSetCursorPosCallback(this->window, Main::_glfwCursorCallback);
    glfwSetKeyCallback(this->window, Main::_glfwInputCallback);
    glfwSetMouseButtonCallback(this->window, Main::_glfwMouseCallback);

    if (glewInit() != GLEW_OK)
        exit(-1);

    /* Set view parameters */
    glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
    glViewport(0, 0, this->screen_width, this->screen_height);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);

    /* Set texturing parameters */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Set debug mode */
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(errorOccurredGL, NULL);

    spige_init(&this->engine);
    spige_viewport(&engine, this->screen_width, this->screen_height);

    LOGI("GL Init: %d", GL_VERSION);

    // internalInit();

    // =====================================================
    // initialize OpenGL ES and EGL

    /*
    * Here specify the attributes of the desired configuration.
    * Below, we select an EGLConfig with at least 8 bits per color
    * component compatible with on-screen windows
    */
    //const EGLint attribs[] = {
    //    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    //    EGL_BLUE_SIZE, 8,
    //    EGL_GREEN_SIZE, 8,
    //    EGL_RED_SIZE, 8,
    //    EGL_DEPTH_SIZE, 24,
    //    EGL_NONE
    //};

    //const EGLint context_attribs[] = {
    //    EGL_CONTEXT_CLIENT_VERSION, 3,
    //    EGL_NONE
    //};

    //EGLint w, h, format;
    //EGLint numConfigs;
    //EGLConfig config;

    //display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    //eglInitialize(display, nullptr, nullptr);

    /* Here, the application chooses the configuration it desires. In this
    * sample, we have a very simplified selection process, where we pick
    * the first EGLConfig that matches our criteria */
    //eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
    * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
    * As soon as we picked a EGLConfig, we can safely reconfigure the
    * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    //eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

    //ANativeWindow_setBuffersGeometry(this->androidApp->window, 0, 0, format);

    //surface = eglCreateWindowSurface(display, config, this->androidApp->window, nullptr);
    //context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);

    //if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
    //    LOGW("Unable to eglMakeCurrent");
    //    return;
    //}

    //eglQuerySurface(display, surface, EGL_WIDTH, &w);
    //eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    //this->state.angle = 0;

    //LOGI("GL Init: %d", GL_VERSION);

    //spige_init(&engine);
    //spige_viewport(&engine, w, h);
    //engine.asset_mgr = this->androidApp->activity->assetManager;
}

void Main::terminateGraphics() {

    spige_destroy(&this->engine);

    //if (this->display != EGL_NO_DISPLAY) {
    //    eglMakeCurrent(this->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    //    if (this->context != EGL_NO_CONTEXT) {
    //        eglDestroyContext(this->display, this->context);
    //    }
    //    if (this->surface != EGL_NO_SURFACE) {
    //        eglDestroySurface(this->display, this->surface);
    //    }
    //    eglTerminate(this->display);
    //}

    //this->display = EGL_NO_DISPLAY;
    //this->context = EGL_NO_CONTEXT;
    //this->surface = EGL_NO_SURFACE;
}

/**
* Process the next input event.
*/
//int32_t Main::androidHandleInput(struct android_app* app, AInputEvent* event) {
//
//    Main *myApp = (Main *) app->userData;
//    int32_t eventType = AInputEvent_getType(event);
//
//    if (eventType == AINPUT_EVENT_TYPE_MOTION) {
//
//        int32_t motionAction = AMotionEvent_getAction(event);
//        int32_t motionType = motionAction & AMOTION_EVENT_ACTION_MASK;
//        size_t pointerCount = AMotionEvent_getPointerCount(event); // may overflow buffer
//
//        if (pointerCount < 1)
//            return 0;
//
//        switch (motionType) {
//            case AMOTION_EVENT_ACTION_MOVE:
//
//                for (size_t i = 0; i < pointerCount; i++) {
//                    myApp->engine.cursor[i][0] = AMotionEvent_getX(event, i);
//                    myApp->engine.cursor[i][1] = AMotionEvent_getY(event, i);
//                }
//
//                return 1;
//
//            case AMOTION_EVENT_ACTION_DOWN:
//            case AMOTION_EVENT_ACTION_POINTER_DOWN:
//
//                for (size_t i = 0; i < pointerCount; i++) {
//                    myApp->engine.cursor[i][0] = AMotionEvent_getX(event, i);
//                    myApp->engine.cursor[i][1] = AMotionEvent_getY(event, i);
//                }
//
//                myApp->mainScene->onEventPointerDown();
//                return 1;
//
//            case AMOTION_EVENT_ACTION_UP:
//            case AMOTION_EVENT_ACTION_POINTER_UP:
//                myApp->mainScene->onEventPointerUp();
//                return 1;
//
//            default: break;
//        }
//
//        return 0;
//
//    } else if (eventType == AINPUT_EVENT_TYPE_KEY) {
//
//        int32_t keyAction = AKeyEvent_getAction(event);
//        int32_t keyCode = AKeyEvent_getKeyCode(event);
//
//        //Meta state holds info regarding whether shift was held, ctrl, alt, etc...
////        int32_t keyMetaState = AKeyEvent_getMetaState(event);
//
//        if (keyCode == AKEYCODE_VOLUME_UP || keyCode == AKEYCODE_VOLUME_DOWN) {
//            return 0;
//        }
//
//        if (keyAction == AKEY_EVENT_ACTION_DOWN) {
//
////            int event_type = 0;
////            char event_key_char = 0;
//
//            switch (keyCode) {
//                case AKEYCODE_BACK:
//                case AKEYCODE_MENU:
//                    if (!myApp->mainScene->onEventBack()) {
//                        ANativeActivity_finish(myApp->androidApp->activity);
//                    }
//
//                    break;
//                case AKEYCODE_DPAD_LEFT:
//                    myApp->mainScene->onEventLeft();
//                    break;
//                case AKEYCODE_DPAD_UP:
//                    myApp->mainScene->onEventUp();
//                    break;
//                case AKEYCODE_DPAD_RIGHT:
//                    myApp->mainScene->onEventRight();
//                    break;
//                case AKEYCODE_DPAD_DOWN:
//                    myApp->mainScene->onEventDown();
//                    break;
//                case AKEYCODE_ENTER:
//                    myApp->mainScene->onEventSelect();
//                    break;
//                default: break;
//            }
//        }
//
//        return 1;
//    }
//
//    return 0;
//}

/**
* Process the next main command.
*/
//void Main::androidHandleCmd(struct android_app* app, int32_t cmd) {
//
//    Main* myApp = (Main*)app->userData;
//
//    switch (cmd) {
//
//        case APP_CMD_SAVE_STATE:
//
//            // The system has asked us to save our current state.  Do so.
//            myApp->androidApp->savedState = malloc(sizeof(Main::SavedState));
//            *((Main::SavedState*)myApp->androidApp->savedState) = myApp->state;
//            myApp->androidApp->savedStateSize = sizeof(Main::SavedState);
//
//            break;
//
//        case APP_CMD_INIT_WINDOW:
//
//            // The window is being shown, get it ready.
//            if (myApp->androidApp->window != nullptr) {
//                myApp->initGraphics();
//                myApp->load();
//            }
//
//            break;
//
//        case APP_CMD_TERM_WINDOW:
//            // The window is being hidden or closed, clean it up.
//
//            myApp->terminateGraphics();
//
//            break;
//
//        case APP_CMD_GAINED_FOCUS:
//            // When our app gains focus, we start monitoring the accelerometer.
//            if (myApp->accelerometerSensor != nullptr) {
//                ASensorEventQueue_enableSensor(myApp->sensorEventQueue,
//                                               myApp->accelerometerSensor);
//                // We'd like to get 60 events per second (in microseconds).
//                ASensorEventQueue_setEventRate(myApp->sensorEventQueue,
//                                               myApp->accelerometerSensor, (1000L / 60) * 1000);
//            }
//
//            myApp->mainScene->resume();
//            myApp->resume();
//
//            break;
//
//        case APP_CMD_LOST_FOCUS:
//            // When our app loses focus, we stop monitoring the accelerometer.
//            // This is to avoid consuming battery while not being used.
//            if (myApp->accelerometerSensor != nullptr) {
//                ASensorEventQueue_disableSensor(myApp->sensorEventQueue,
//                                                myApp->accelerometerSensor);
//            }
//
//            // Also stop animating.
//            myApp->mainScene->pause();
//            myApp->pause();
//
//            break;
//
//        default: return;
//    }
//}

/**
* This is the main entry point
*/

#if defined (WIN32) || defined (_WIN32)
#include <Windows.h>

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR lpCmdLine,
    _In_ int nCmdShow) {

    (void)hInstance, (void)hPrevInstance, (void)lpCmdLine, (void)nCmdShow;

    Main main(__argc, __argv);
    main.run();
}
#else
int main(int argc, char* argv[]) {
    Main main(argc, argv);
    main.run();
}
#endif
