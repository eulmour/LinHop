#ifndef LINHOP_GAMEOBJECT_HPP
#define LINHOP_GAMEOBJECT_HPP

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

class IGameObject {

public:
    virtual ~IGameObject() = default;
    virtual void draw() = 0;
    virtual void activate() = 0;
    virtual void deactivate() = 0;

protected:
    glm::vec2 pos {0.f};
    glm::vec4 color {1.f};
};

#endif //LINHOP_GAMEOBJECT_HPP
