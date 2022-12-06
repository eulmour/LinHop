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

class Input {

public:
	enum class State {
		Off, Pressed, Hold, Released
	};

	enum class Key : int {
		Pointer = 0, PointerMove,
		Up , Down, Left, Right,
		A, B, X, Y,
		Select, Back,
		Ctrl,
		EOL
	};

    using PointerArray = std::array<std::array<float, 2>, ENGINE_POINTER_COUNT_MAX>;
    using InputArray = std::array<State, static_cast<int>(Key::EOL)>;

    Input();
    void clearStates();
    bool isKeyDown(Key key);
    bool isKeyHold(Key key);
    bool isKeyUp(Key key);
    [[nodiscard]] State& get(Key key);
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
