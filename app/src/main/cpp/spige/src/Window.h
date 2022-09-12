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

#include <functional>

class WindowConfig {

public:
//    WindowConfig& setSizeCallback(std::function<void(int, int)> callback);
//    WindowBuilder& setPointerCallback();
//    WindowBuilder& setKeyCallback();
//    WindowBuilder& setMouseButtonCallback();
};

class Window {

public:
    explicit Window(WindowConfig config);
    ~Window();

    void close();
    bool isShouldClose();
    void swapBuffers();

    float getDeltaTime();
    [[nodiscard]] float getDeltaTimeLast() const;
    [[nodiscard]] IVec2 getLogicalSize() const { return logicalSize; };
    void setLogicalSize(IVec2 size) { this->logicalSize = size; }

protected:
    IVec2 logicalSize {480, 800};

    struct FrameInfo {
        float deltaTime {1.f / 60.f};
        float lastFrameTime {1.f / 60.f};
    } frameInfo;

#if defined(__ANDROID__) || defined(ANDROID)
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
#else
    GLFWwindow* glfwWindow{};

    static void glfwSizeCallback_(GLFWwindow* window, int width, int height);

#endif
};

#endif //SPIGE_WINDOW_H
