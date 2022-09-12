#include "Window.h"
#include "Framework.h"
#include "Engine.h"
#include "Input.h"

#if defined(__ANDROID__) || defined(ANDROID)

/**
* Initialize an EGL context for the current display.
*/
void Window::init() {
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

void Window::terminate() {

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

#else

void Window::glfwSizeCallback_(GLFWwindow *window, int width, int height) {
    Engine::instance->window->setLogicalSize({width,height});
    Engine::instance->graphics.viewport({width, height});
}

Window::Window(WindowConfig config) {

    /* Initialize the library */
    if (!glfwInit())
        exit(-1);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

#if defined(__linux__)

    if (glfwPlatformSupported(GLFW_PLATFORM_WAYLAND))
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
    else if (glfwPlatformSupported(GLFW_PLATFORM_X11))
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif

    /* Create a windowed mode window and its OpenGL context */
    this->glfwWindow = glfwCreateWindow(this->getLogicalSize()[0], this->getLogicalSize()[1], "LinHop", NULL, NULL);

    if (!this->glfwWindow)
    {
        glfwTerminate();
        exit(-1);
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(this->glfwWindow);
    glfwSetFramebufferSizeCallback(this->glfwWindow, Window::glfwSizeCallback_);
    glfwSwapInterval(1);
    glfwSetCursorPosCallback(this->glfwWindow, Input::glfwCursorCallback_);
    glfwSetKeyCallback(this->glfwWindow, Input::glfwInputCallback_);
    glfwSetMouseButtonCallback(this->glfwWindow, Input::glfwMouseCallback_);

    if (glewInit() != GLEW_OK)
        exit(-1);

    LOGI("GL Init: %d\n", GL_VERSION);
}

Window::~Window() {

}

void Window::close() {
    glfwSetWindowShouldClose(this->glfwWindow, 1);
}

bool Window::isShouldClose() {
    return glfwWindowShouldClose(this->glfwWindow);
}

void Window::swapBuffers() {
    glfwSwapBuffers(this->glfwWindow);
}

float Window::getDeltaTime() {
//    auto currentFrame = static_cast<float>(glfwGetTime());
//    this->frameInfo.deltaTime = currentFrame - this->frameInfo.lastFrameTime;
//    this->frameInfo.lastFrameTime = currentFrame;
//    this->frameInfo.deltaTime = this->frameInfo.lastFrameTime = 1.f / 60.f;
    return this->frameInfo.deltaTime;
}

float Window::getDeltaTimeLast() const { return this->frameInfo.lastFrameTime; }

#endif
