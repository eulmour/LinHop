#include "Window.h"

#include <utility>
#include "Framework.h"
#include "Engine.h"
#include "Input.h"

#if defined(__ANDROID__) || defined(ANDROID)

/**
* Initialize an EGL context for the current display.
*/
Window::Window(const Config& config) {
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
    EGLConfig eglConfig;

    m_AndroidApp = config.m_AndroidApp;

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, nullptr, nullptr);

    /* Here, the application chooses the configuration it desires. In this
    * sample, we have a very simplified selection process, where we pick
    * the first EGLConfig that matches our criteria */
    eglChooseConfig(display, attribs, &eglConfig, 1, &numConfigs);

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
    * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
    * As soon as we picked a EGLConfig, we can safely reconfigure the
    * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    eglGetConfigAttrib(display, eglConfig, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(m_AndroidApp->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, eglConfig, m_AndroidApp->window, nullptr);
    context = eglCreateContext(display, eglConfig, EGL_NO_CONTEXT, context_attribs);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGW("Unable to eglMakeCurrent");
        return;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    this->setLogicalSize({w, h});

    LOGI("GL Init: %d", GL_VERSION);
}

Window::~Window() {

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

void Window::close() {
    m_ShouldClose = true;
    ANativeActivity_finish(m_AndroidApp->activity);
}

bool Window::isShouldClose() {
    return m_ShouldClose;
}

void Window::swapBuffers() {
    eglSwapBuffers(this->display, this->surface);
}

float Window::getDeltaTime() {
    return this->frameInfo.deltaTime;
}

Window::Config &Window::Config::androidApp(android_app *androidApp) {
    this->m_AndroidApp = androidApp;
    return *this;
}

#else

void Window::glfwSizeCallback_(GLFWwindow *window, int width, int height) {
    Engine::instance->window->setLogicalSize({width,height});
    Engine::instance->graphics.viewport({width, height});
}

Window::Window(const Config& config) {

    /* Initialize the library */
    if (!glfwInit())
        exit(-1);

    this->setLogicalSize(config.m_InnerSize);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, config.m_Resizeable ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_MAXIMIZED, config.m_Maximized ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, config.m_Visible ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, config.m_Decorated ? GLFW_TRUE : GLFW_FALSE);

#if defined(__linux__)
    if (glfwPlatformSupported(GLFW_PLATFORM_WAYLAND))
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
    else if (glfwPlatformSupported(GLFW_PLATFORM_X11))
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif

    /* Create a windowed mode window and its OpenGL context */
    if (!config.m_Fullscreen) {
        this->glfwWindow = glfwCreateWindow(
            config.m_InnerSize[0],
            config.m_InnerSize[1],
            config.m_Title.c_str(), nullptr, nullptr);
    } else {

        GLFWmonitor* monitor =  glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        this->glfwWindow = glfwCreateWindow(
            mode->width,
            mode->height,
            config.m_Title.c_str(), nullptr, nullptr);
    }

    if (!this->glfwWindow)
    {
        glfwTerminate();
        exit(-1);
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(this->glfwWindow);
    glfwSetFramebufferSizeCallback(this->glfwWindow, Window::glfwSizeCallback_);
    glfwSwapInterval(config.m_Vsync ? 1 : 0);
    glfwSetCursorPosCallback(this->glfwWindow, Input::glfwCursorCallback_);
    glfwSetKeyCallback(this->glfwWindow, Input::glfwInputCallback_);
    glfwSetMouseButtonCallback(this->glfwWindow, Input::glfwMouseCallback_);

    if (glewInit() != GLEW_OK)
        exit(-1);

    LOGI("GL Init: %d\n", GL_VERSION);
}

Window::~Window() = default;

void Window::close() {
    m_ShouldClose = true;
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

#endif

float Window::getDeltaTimeLast() const { return this->frameInfo.lastFrameTime; }

Window::Config &Window::Config::title(std::string title) {
    this->m_Title = std::move(title);
    return *this;
}

Window::Config &Window::Config::innerSize(int width, int height) {
    this->m_InnerSize = {width, height};
    return *this;
}

Window::Config &Window::Config::resizeable(bool resizeable) {
    this->m_Resizeable = resizeable;
    return *this;
}

Window::Config &Window::Config::fullscreen(bool fullscreen) {
    this->m_Fullscreen = fullscreen;
    return *this;
}

Window::Config &Window::Config::maximized(bool maximized) {
    this->m_Maximized = maximized;
    return *this;
}

Window::Config &Window::Config::vsync(bool vsync) {
    this->m_Vsync = vsync;
    return *this;
}

Window::Config &Window::Config::visible(bool visible) {
    this->m_Visible = visible;
    return *this;
}

Window::Config &Window::Config::decorated(bool decorated) {
    this->m_Decorated = decorated;
    return *this;
}
