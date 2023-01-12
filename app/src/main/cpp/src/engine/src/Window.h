#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#include "Framework.h"

#if defined(__ANDROID__) || defined(ANDROID)
#include <android/sensor.h>
#include <EGL/egl.h>
#else
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#endif

#include <string>
#include <functional>

class Window {

public:
    struct Config {

        Config& title(std::string title);
        Config& innerSize(int width, int height);
        Config& resizeable(bool resizeable);
        Config& fullscreen(bool fullscreen);
        Config& maximized(bool maximized);
        Config& vsync(bool vsync);
        Config& visible(bool visible);
        Config& decorated(bool decorated);
        Config& userPointer(void* ptr);

        [[nodiscard]] std::string title() const { return this->title_; }
        [[nodiscard]] IVec2 innerSize() const { return this->inner_size_; }
        [[nodiscard]] bool resizeable() const { return this->resizeable_; }
        [[nodiscard]] bool fullscreen() const { return this->fullscreen_; }
        [[nodiscard]] bool maximized() const { return this->maximized_; }
        [[nodiscard]] bool vsync() const { return this->vsync_; }
        [[nodiscard]] bool visible() const{ return this->visible_; }
        [[nodiscard]] bool decorated() const { return this->decorated_; }
        [[nodiscard]] void* userPointer() const { return this->user_ptr_; }

    private:
        std::string title_{"Application"};
        IVec2 inner_size_{480, 800};
        bool resizeable_{true};
        bool fullscreen_{false};
        bool maximized_{false};
        bool vsync_{false};
        bool visible_{true};
        bool decorated_{true};
        void* user_ptr_{nullptr};

#if defined(__ANDROID__) || defined(ANDROID)
    public:
        Config& androidApp(android_app* androidApp);
        [[nodiscard]] android_app* androidApp() const { return this->m_AndroidApp; }
    private:
        android_app* m_AndroidApp;
#endif
    };

    explicit Window(const Config& config);
    ~Window();

    void close();
    bool isShouldClose();
    bool isFocused() const { return this->focused; }
    void swapBuffers();

    float delta_time();
    [[nodiscard]] float delta_time_last() const;
    IVec2 size();
    void size(IVec2 size);
    [[nodiscard]] IVec2 logical_size() const { return logical_size_; };
    void logical_size(IVec2 size) { this->logical_size_ = size; }
    void setFocused(bool flag) { this->focused = flag; }

protected:
    IVec2 logical_size_{};
    bool focused{true};
    bool should_close{false};

    struct FrameInfo {
        float delta_time {1.f / 60.f};
        float last_frame_time {1.f / 60.f};
    } frameInfo;

#if defined(__ANDROID__) || defined(ANDROID)
    android_app* android_app_ptr;
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
#else
    GLFWwindow* glfw_window{};

    static void glfwSizeCallback_(GLFWwindow* window, int width, int height);
    static void glfwFocusCallback_(GLFWwindow* window, int focused);

#endif
};

#endif //ENGINE_WINDOW_H
