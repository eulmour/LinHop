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

namespace wuh {

class Window {

public:
    struct Config {

        Config& title(std::string title);
        Config& size(int width, int height);
        Config& resizeable(bool resizeable);
        Config& fullscreen(bool fullscreen);
        Config& maximized(bool maximized);
        Config& vsync(bool vsync);
        Config& visible(bool visible);
        Config& decorated(bool decorated);
        Config& user_pointer(void* ptr);

        [[nodiscard]] std::string title() const { return this->title_; }
        [[nodiscard]] IVec2 size() const { return this->size_; }
        [[nodiscard]] bool resizeable() const { return this->resizeable_; }
        [[nodiscard]] bool fullscreen() const { return this->fullscreen_; }
        [[nodiscard]] bool maximized() const { return this->maximized_; }
        [[nodiscard]] bool vsync() const { return this->vsync_; }
        [[nodiscard]] bool visible() const{ return this->visible_; }
        [[nodiscard]] bool decorated() const { return this->decorated_; }
        [[nodiscard]] void* user_pointer() const { return this->user_ptr_; }

    private:
        std::string title_{"Application"};
        IVec2 size_{450, 800};
        bool resizeable_{true};
        bool fullscreen_{false};
        bool maximized_{false};
        bool vsync_{false};
        bool visible_{true};
        bool decorated_{true};
        void* user_ptr_{nullptr};

#if defined(__ANDROID__) || defined(ANDROID)
    public:
        Config& android_app_ptr(android_app* app_ptr);
        [[nodiscard]] struct android_app* android_app_ptr() const { return this->android_app_; }
    private:
        struct android_app* android_app_;
#endif
    };

    explicit Window(const Config& config);
    ~Window();

    void close();
    bool should_close() const;
    bool focused() const { return this->focused_; }
    void swap_buffers();

    float delta_time() const;
    [[nodiscard]] float delta_time_last() const;
    [[nodiscard]] IVec2 size() { return logical_size_; }
    [[nodiscard]] IVec2 physical_size() const { return physical_size_; };
    void focused(bool flag) { this->focused_ = flag; }

#ifdef __EMSCRIPTEN__
    void emscripten_force_size(IVec2 size);
#endif

protected:
    IVec2 logical_size_{};
    IVec2 physical_size_{};
    bool focused_{true};
    bool should_close_{false};

    struct FrameInfo {
        float delta_time {1.f / 60.f};
        float last_frame_time {1.f / 60.f};
    } frame_info_;

#if defined(__ANDROID__) || defined(ANDROID)
    android_app* android_app_;
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
#else
    GLFWwindow* glfw_window{};

    static void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void glfw_window_size_callback(GLFWwindow* window, int width, int height);
    static void glfw_window_focus_callback(GLFWwindow* window, int focused);

#endif
};

} // end of namespace wuh

#endif //ENGINE_WINDOW_H