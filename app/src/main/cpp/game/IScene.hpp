#ifndef LINHOP_ISCENE_HPP
#define LINHOP_ISCENE_HPP

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"
#include "glm/gtc/type_ptr.hpp"

class IScene {
public:
    IScene() = default;
    virtual ~IScene() = default;

    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual bool draw() = 0;
    virtual void reset() = 0;

    virtual void onEventPointerMove() = 0;
    virtual void onEventPointerDown() = 0;
    virtual void onEventPointerUp() = 0;
    virtual void onEventSelect() = 0;
    virtual void onEventUp() = 0;
    virtual void onEventLeft() = 0;
    virtual void onEventDown() = 0;
    virtual void onEventRight() = 0;
    virtual bool onEventBack() = 0;

    glm::vec4 backgroundColor{0.0f, 0.1f, 0.2f, 1.f};

protected:
    virtual void update(float dt) = 0;

    bool pressed = false;
    bool pressedOnce = false;
    glm::vec2 prevMousePos {0};
    glm::vec2 lastClick {0};
};

#endif //LINHOP_ISCENE_HPP
