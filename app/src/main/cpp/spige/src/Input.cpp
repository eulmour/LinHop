#include "Input.h"
#include "Engine.h"

#include <algorithm>

static std::array<int, static_cast<std::size_t>(InputKey::EOL)> matchKey {
    GLFW_KEY_UP,
    GLFW_KEY_DOWN,
    GLFW_KEY_LEFT,
    GLFW_KEY_RIGHT,
    GLFW_KEY_ENTER,
    GLFW_KEY_ESCAPE,
};

void Input::glfwCursorCallback_(GLFWwindow *window, double xpos, double ypos)
{
    Engine::instance->input.getPointerArray()[0][0] = static_cast<float>(xpos);
    Engine::instance->input.getPointerArray()[0][1] = static_cast<float>(ypos);
    Engine::instance->input.get(InputKey::PointerMove) = InputState::Hold;
}

void Input::glfwInputCallback_(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    InputState newState;

    switch (action) {
        case GLFW_PRESS: newState = InputState::Pressed; break;
        case GLFW_RELEASE: newState = InputState::Released; break;
        default: return;
    }

    auto& state = Engine::instance->input.get(static_cast<InputKey>(::matchKey[key]));

    if (state == newState || (state == InputState::Hold && newState == InputState::Pressed)) {

    } else {
        state = newState;
    }

//    switch (key)
//    {
//        case GLFW_KEY_LEFT:
//            Engine::instance->input.get(InputKey::Left) = inputAction;
//            break;
//        case GLFW_KEY_UP:
//            Engine::instance->input.get(InputKey::Up) = inputAction;
//            break;
//        case GLFW_KEY_RIGHT:
//            Engine::instance->input.get(InputKey::Right) = inputAction;
//            break;
//        case GLFW_KEY_DOWN:
//            Engine::instance->input.get(InputKey::Down) = inputAction;
//            break;
//        case GLFW_KEY_ENTER:
//            Engine::instance->input.get(InputKey::Select) = inputAction;
//            break;
//        case GLFW_KEY_ESCAPE:
//            Engine::instance->input.get(InputKey::Back) = inputAction;
//            break;
//        default:
//            break;
//    }
}

void Input::glfwMouseCallback_(GLFWwindow *window, int button, int action, int mods)
{
    Engine::instance->input.get(InputKey::PointerMove) = InputState::Hold;

    if (action != GLFW_PRESS && action != GLFW_RELEASE)
        return;

    InputState inputAction = GLFW_PRESS
        ? InputState::Pressed
        : InputState::Released;

    switch (button)
    {
        case GLFW_MOUSE_BUTTON_LEFT:
            // TODO check
//            Engine::instance->input.get(InputKey::Pointer) = inputAction;
            Engine::instance->input.get(InputKey::Pointer) = InputState::Released;
            break;
    }
}

Input::Input() {
    this->clearStates();
}

void Input::clearStates() {
    this->inputs.fill(InputState::Off);
}

bool Input::isKeyDown(InputKey key) {
    return this->get(key) == InputState::Pressed;
}

bool Input::isKeyHold(InputKey key) {
    return this->get(key) == InputState::Hold;
}

bool Input::isKeyUp(InputKey key) {
    return this->get(key) == InputState::Released;
}

InputState& Input::get(InputKey key) {
    return this->inputs[static_cast<int>(key)];
}
