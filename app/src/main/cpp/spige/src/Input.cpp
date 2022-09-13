#include "Input.h"
#include "Engine.h"

#include <algorithm>
#include <unordered_map>
#include <stdexcept>

#if defined(__ANDROID__) || defined(ANDROID)

int32_t Input::androidHandleInput(struct android_app* app, AInputEvent* event) {

    auto *pEngine = (Engine *) app->userData;
    int32_t eventType = AInputEvent_getType(event);

    if (eventType == AINPUT_EVENT_TYPE_MOTION) {

        int32_t motionAction = AMotionEvent_getAction(event);
        int32_t motionType = motionAction & AMOTION_EVENT_ACTION_MASK;
        size_t pointerCount = AMotionEvent_getPointerCount(event); // TODO may overflow buffer
        auto& pointerArray = pEngine->input.getPointerArray();

        if (pointerCount < 1)
            return 0;

        switch (motionType) {
            case AMOTION_EVENT_ACTION_MOVE:

                for (size_t i = 0; i < pointerCount; i++) {
                    pointerArray[i][0] = AMotionEvent_getX(event, i);
                    pointerArray[i][1] = AMotionEvent_getY(event, i);
                }

                pEngine->input.get(InputKey::PointerMove) = InputState::Hold;
                return 1;

            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:

                for (size_t i = 0; i < pointerCount; i++) {
                    pointerArray[i][0] = AMotionEvent_getX(event, i);
                    pointerArray[i][1] = AMotionEvent_getY(event, i);
                }

                try {
                    auto& oldState = pEngine->input.get(InputKey::Pointer);

                    if (oldState != InputState::Pressed) {
                        oldState = InputState::Pressed;
                    }

                } catch (std::out_of_range& e) {
                    LOGE("Touch: %s", e.what());
                }

                return 1;

            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:

                try {
                    auto& oldState = pEngine->input.get(InputKey::Pointer);

                    if (oldState != InputState::Released) {
                        oldState = InputState::Released;
                    }

                } catch (std::out_of_range& e) {
                            LOGE("Touch: %s", e.what());
                }
                return 1;

            default: break;
        }

        return 0;

    } else if (eventType == AINPUT_EVENT_TYPE_KEY) {

        int32_t keyAction = AKeyEvent_getAction(event);
        int32_t keyCode = AKeyEvent_getKeyCode(event);

        //Meta state holds info regarding whether shift was held, ctrl, alt, etc...
//        int32_t keyMetaState = AKeyEvent_getMetaState(event);

        if (keyCode == AKEYCODE_VOLUME_UP || keyCode == AKEYCODE_VOLUME_DOWN) {
            return 0;
        }

        InputState newState;
        switch (keyAction) {
            case AKEY_EVENT_ACTION_DOWN: newState = InputState::Pressed; break;
            case AKEY_EVENT_ACTION_UP: newState = InputState::Released; break;
            default: return 0;
        }

        static const std::unordered_map<int, InputKey> matchKey {
            {AKEYCODE_DPAD_UP, InputKey::Up},
            {AKEYCODE_DPAD_DOWN, InputKey::Down},
            {AKEYCODE_DPAD_LEFT, InputKey::Left},
            {AKEYCODE_DPAD_RIGHT, InputKey::Right},
            {AKEYCODE_ENTER, InputKey::Select},
            {AKEYCODE_BACK, InputKey::Back},
            {AKEYCODE_MENU, InputKey::Back},
        };

        try {
            auto keyFound = matchKey.at(keyCode);
            auto& oldState = pEngine->input.get(keyFound);

//            if (oldState == newState || (oldState == InputState::Hold && newState == InputState::Pressed)) {
//            } else {
//                oldState = newState;
//            }

            if (oldState != newState || (oldState != InputState::Hold && newState != InputState::Pressed)) {
                oldState = newState;
            }

        } catch (std::out_of_range& e) {
            LOGE("Input: %s", e.what());
        }

//        if (keyAction == AKEY_EVENT_ACTION_DOWN) {
//
//            switch (keyCode) {
//                case AKEYCODE_BACK:
//                case AKEYCODE_MENU:
//                    if (!pEngine->currentScene->onEventBack()) {
//                    }
//
//                    break;
//                case AKEYCODE_DPAD_LEFT:
//                    pEngine->currentScene->onEventLeft();
//                    break;
//                case AKEYCODE_DPAD_UP:
//                    pEngine->currentScene->onEventUp();
//                    break;
//                case AKEYCODE_DPAD_RIGHT:
//                    pEngine->currentScene->onEventRight();
//                    break;
//                case AKEYCODE_DPAD_DOWN:
//                    pEngine->currentScene->onEventDown();
//                    break;
//                case AKEYCODE_ENTER:
//                    pEngine->currentScene->onEventSelect();
//                    break;
//                default: break;
//            }
//        }

        return 1;
    }

    return 0;
}

#else
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

    static const std::unordered_map<int, InputKey> matchKey {
        {GLFW_KEY_UP, InputKey::Up},
        {GLFW_KEY_DOWN, InputKey::Down},
        {GLFW_KEY_LEFT, InputKey::Left},
        {GLFW_KEY_RIGHT, InputKey::Right},
        {GLFW_KEY_ENTER, InputKey::Select},
        {GLFW_KEY_ESCAPE, InputKey::Back}
    };

    try {
        auto keyFound = matchKey.at(key);
        auto& oldState = Engine::instance->input.get(keyFound);

        if (oldState == newState || (oldState == InputState::Hold && newState == InputState::Pressed)) {

        } else {
            oldState = newState;
        }
    } catch (std::out_of_range& e) {
        LOGE("Input: %s", e.what());
    }
}

void Input::glfwMouseCallback_(GLFWwindow *window, int button, int action, int mods)
{
    InputState newState;

    switch (action) {
        case GLFW_PRESS: newState = InputState::Pressed; break;
        case GLFW_RELEASE: newState = InputState::Released; break;
        default: return;
    }

    static const std::unordered_map<int, InputKey> matchButton {
        {GLFW_MOUSE_BUTTON_LEFT, InputKey::Pointer},
    };

    try {
        auto keyFound = matchButton.at(button);
        auto& oldState = Engine::instance->input.get(keyFound);

        if (oldState == newState || (oldState == InputState::Hold && newState == InputState::Pressed)) {

        } else {
            oldState = newState;
        }
    } catch (std::out_of_range& e) {
        LOGE("Mouse: %s", e.what());
    }
}
#endif

Input::Input() {
    this->pointers.fill(std::array<float, 2>{0.f, 0.f});
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
    auto iKey = static_cast<int>(key);

    if (iKey < 0 || iKey > static_cast<int>(InputKey::EOL))
        throw std::out_of_range("Key not found");

    return this->inputs[static_cast<int>(key)];
}
