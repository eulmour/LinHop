#ifndef ENGINE_INPUT_H
#define ENGINE_INPUT_H

#if defined(__ANDROID__) || defined(ANDROID)
#include <android/sensor.h>
#include <EGL/egl.h>
#else
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#endif

#include "Framework.h"
#include <array>

enum class InputState {
    Off, Pressed, Hold, Released
};

enum class InputKey : int {
    Up = 0, Down, Left, Right, Select, Back, Pointer, PointerMove, EOL
};

class Input {

public:
    using PointerArray = std::array<std::array<float, 2>, ENGINE_POINTER_COUNT_MAX>;
    using InputArray = std::array<InputState, static_cast<int>(InputKey::EOL)>;

    Input();
    void clearStates();
    bool isKeyDown(InputKey key);
    bool isKeyHold(InputKey key);
    bool isKeyUp(InputKey key);
    [[nodiscard]] InputState& get(InputKey key);
    [[nodiscard]] InputArray& getInputArray() { return inputs; }
    [[nodiscard]] PointerArray& getPointerArray() { return pointers; }

#if defined(__ANDROID__) || defined(ANDROID)
    static int32_t androidHandleInput(android_app* app, AInputEvent* event);
#else
    static void glfwCursorCallback_(GLFWwindow* window, double xpos, double ypos);
    static void glfwInputCallback_(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void glfwMouseCallback_(GLFWwindow* window, int button, int action, int mods);
#endif

protected:

    InputArray inputs{};
    PointerArray pointers{}; // pointer count
};

#endif //ENGINE_INPUT_H
