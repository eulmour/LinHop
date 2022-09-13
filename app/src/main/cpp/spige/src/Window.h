#ifndef SPIGE_WINDOW_H
#define SPIGE_WINDOW_H

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

        std::string m_Title{"Application"};
        IVec2 m_InnerSize{480, 800};
        bool m_Resizeable{true};
        bool m_Fullscreen{false};
        bool m_Maximized{false};
        bool m_Vsync{false};
        bool m_Visible{true};
        bool m_Decorated{true};

#if defined(__ANDROID__) || defined(ANDROID)
        Config& androidApp(android_app* androidApp);
        android_app* m_AndroidApp;
#endif
    };

    explicit Window(const Config& config);
    ~Window();

    void close();
    bool isShouldClose();
    void swapBuffers();

    float getDeltaTime();
    [[nodiscard]] float getDeltaTimeLast() const;
    [[nodiscard]] IVec2 getLogicalSize() const { return logicalSize; };
    void setLogicalSize(IVec2 size) { this->logicalSize = size; }

protected:
    IVec2 logicalSize{};
    bool m_ShouldClose{false};

    struct FrameInfo {
        float deltaTime {1.f / 60.f};
        float lastFrameTime {1.f / 60.f};
    } frameInfo;

#if defined(__ANDROID__) || defined(ANDROID)
    android_app* m_AndroidApp;
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
#else
    GLFWwindow* glfwWindow{};

    static void glfwSizeCallback_(GLFWwindow* window, int width, int height);

#endif
};

#endif //SPIGE_WINDOW_H
