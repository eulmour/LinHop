#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glm/glm.hpp>
#include "Texture.h"

class IObject
{
public:
    virtual ~IObject() {}
    virtual void Draw() = 0;
};

class GameObject : public IObject
{
public:

    glm::vec2   Position, Size, Velocity;
    glm::vec3   Color;
    float       Rotation;
    bool        IsSolid;
    bool        Destroyed;
    Texture     Sprite;

    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, Texture sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

    //virtual void Draw(SpriteRenderer& renderer);
    virtual void Draw() override;
};

class CircleObject : public GameObject
{
public:

    float   Radius;
    bool    Stuck;
    bool    Sticky, PassThrough;

    CircleObject();
    CircleObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture sprite);
    glm::vec2 Move(float dt, unsigned int window_width);
    void      Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif