#include "Window.h"

#include <utility>
#include "Framework.h"
#include "Engine.h"
#include "Input.h"
#include <stdexcept>

namespace wuh {

#if defined(__ANDROID__) || defined(ANDROID)

extern struct android_app* android_app;

int android_get_density() {

    AConfiguration* config = AConfiguration_new();
    AConfiguration_fromAssetManager(config, android_app->activity->assetManager);

    int density = AConfiguration_getDensity(config);
    if (density >= ACONFIGURATION_DENSITY_XXXHIGH) {
        return 4;
    } else if (density >= ACONFIGURATION_DENSITY_XXHIGH) {
        return 3;
    } else if (density >= ACONFIGURATION_DENSITY_XHIGH) {
        return 2;
    } else {
        return 1;
    }
}

/**
* Initialize an EGL context for the current display.
*/
Window::Window(const Config& config) {
    /*
    * Here specify the attributes of the desired configuration.
    * Below, we select an EGLConfig with at least 8 bits per color
    * component compatible with on-screen windows
    */
    const int density = android_get_density();

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

    this->android_app_ = config.android_app_ptr();

    this->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(this->display, nullptr, nullptr);

    /* Here, the application chooses the configuration it desires. In this
    * sample, we have a very simplified selection process, where we pick
    * the first EGLConfig that matches our criteria */
    eglChooseConfig(display, attribs, &eglConfig, 1, &numConfigs);

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
    * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
    * As soon as we picked a EGLConfig, we can safely reconfigure the
    * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    eglGetConfigAttrib(display, eglConfig, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(this->android_app_->window, 0, 0, format);

    this->surface = eglCreateWindowSurface(this->display, eglConfig, this->android_app_->window, nullptr);
    this->context = eglCreateContext(this->display, eglConfig, EGL_NO_CONTEXT, context_attribs);

    if (eglMakeCurrent(this->display, this->surface, this->surface, this->context) == EGL_FALSE) {
        throw std::runtime_error("eglMakeCurrent failed");
    }

    eglQuerySurface(this->display, this->surface, EGL_WIDTH, &w);
    eglQuerySurface(this->display, this->surface, EGL_HEIGHT, &h);

    this->physical_size_ = {w, h};
    this->logical_size_ = {w / density, h / density};

    Graphics::catch_error();

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
    this->should_close_ = true;
    ANativeActivity_finish(android_app_->activity);
}

bool Window::should_close() const {
    return this->should_close_;
}

void Window::swap_buffers() {
    eglSwapBuffers(this->display, this->surface);
}

float Window::delta_time() const {
    return frame_info_.delta_time;
}

Window::Config &Window::Config::android_app_ptr(struct android_app* app_ptr) {
    this->android_app_ = app_ptr;
    return *this;
}

#else

void Window::glfw_window_size_callback(GLFWwindow* window, int width, int height) {
	auto* e = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
    e->window->logical_size_ = {width, height};
}

void Window::glfw_framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	auto* e = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
    e->graphics.viewport({width, height});
    e->window->physical_size_ = {width, height};
}

void Window::glfw_window_focus_callback(GLFWwindow* window, int focused) {
	auto* e = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));

    if (!focused && !!glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
		e->window->focused(true);
        return;
    }

    e->window->focused(!!focused);
}

Window::Window(const Config& config) {

    /* Initialize the library */
    if (!glfwInit()) {
        throw std::runtime_error("glfwInit failed");
    }

#ifdef __EMSCRIPTEN__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#elif __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#   ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#   endif
#endif

    glfwWindowHint(GLFW_RESIZABLE, config.resizeable() ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_MAXIMIZED, config.maximized() ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, config.visible() ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, config.decorated() ? GLFW_TRUE : GLFW_FALSE);

#if defined(__linux__)
    if (glfwPlatformSupported(GLFW_PLATFORM_WAYLAND))
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
    else if (glfwPlatformSupported(GLFW_PLATFORM_X11))
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif

    /* Create a windowed mode window and its OpenGL context */
    if (!config.fullscreen()) {
        this->glfw_window = glfwCreateWindow(
            config.size()[0],
            config.size()[1],
            config.title().c_str(), nullptr, nullptr);
    } else {

        GLFWmonitor* monitor =  glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        this->glfw_window = glfwCreateWindow(
            mode->width,
            mode->height,
            config.title().c_str(), nullptr, nullptr);
    }

    if (!this->glfw_window) {
        glfwTerminate();
        throw std::runtime_error("glfwCreateWindow failed");
    }

    if (config.user_pointer() != nullptr) {
        glfwSetWindowUserPointer(this->glfw_window, config.user_pointer());
    }

    // Make the window's context current
    glfwMakeContextCurrent(this->glfw_window);
    glfwSetWindowSizeCallback(this->glfw_window, Window::glfw_window_size_callback);
    glfwSetFramebufferSizeCallback(this->glfw_window, Window::glfw_framebuffer_size_callback);
    glfwSetCursorPosCallback(this->glfw_window, Input::glfwCursorCallback_);
    glfwSetKeyCallback(this->glfw_window, Input::glfwInputCallback_);
    glfwSetMouseButtonCallback(this->glfw_window, Input::glfwMouseCallback_);
    glfwSetWindowFocusCallback(this->glfw_window, Window::glfw_window_focus_callback);
    
    glfwGetWindowSize(this->glfw_window, &this->logical_size_[0], &this->logical_size_[1]);
    glfwGetFramebufferSize(this->glfw_window, &this->physical_size_[0], &physical_size_[1]);

#ifndef __EMSCRIPTEN__
    glfwSwapInterval(config.vsync() ? 1 : 0);
    glewExperimental = GL_TRUE;
#endif

    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        throw std::runtime_error("glewInit failed");
    }

    Graphics::catch_error();
    LOGI("GL Init: %d", GL_VERSION);
}

Window::~Window() = default;

void Window::close() {
    should_close_ = true;
    glfwSetWindowShouldClose(this->glfw_window, 1);
}

bool Window::should_close() const {
    return glfwWindowShouldClose(this->glfw_window);
}

void Window::swap_buffers() {
    glfwSwapBuffers(this->glfw_window);
    auto current = static_cast<float>(glfwGetTime());
    frame_info_.delta_time = current - frame_info_.last_frame_time;
    frame_info_.last_frame_time = current;
//    frame_info_.delta_time = frame_info_.last_frame_time = 1.f / 60.f;
}

float Window::delta_time() const {
    return frame_info_.delta_time;
}

#endif

#ifdef __EMSCRIPTEN__
void Window::emscripten_force_size(IVec2 size) {
    glfwSetWindowSize(this->glfw_window, size[0], size[1]);
    logical_size_ = physical_size_ = size;
}
#endif

float Window::delta_time_last() const { return this->frame_info_.last_frame_time; }

Window::Config &Window::Config::title(std::string title) {
    this->title_ = std::move(title);
    return *this;
}

Window::Config &Window::Config::size(int width, int height) {
    this->size_ = {width, height};
    return *this;
}

Window::Config &Window::Config::resizeable(bool resizeable) {
    this->resizeable_ = resizeable;
    return *this;
}

Window::Config &Window::Config::fullscreen(bool fullscreen) {
    this->fullscreen_ = fullscreen;
    return *this;
}

Window::Config &Window::Config::maximized(bool maximized) {
    this->maximized_ = maximized;
    return *this;
}

Window::Config &Window::Config::vsync(bool vsync) {
    this->vsync_ = vsync;
    return *this;
}

Window::Config &Window::Config::visible(bool visible) {
    this->visible_ = visible;
    return *this;
}

Window::Config &Window::Config::decorated(bool decorated) {
    this->decorated_ = decorated;
    return *this;
}

Window::Config &Window::Config::user_pointer(void* ptr) {
    this->user_ptr_ = ptr;
    return *this;
}

} // end of namespace wuh