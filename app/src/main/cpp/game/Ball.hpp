#ifndef LINHOP_BALL_HPP
#define LINHOP_BALL_HPP

#include "spige.h"
#include "Lines.hpp"

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

class Ball {
public:

    float radius = 18.0f;
    float diameter = radius / 2;
    float gravity = 9.8f;
    glm::vec2 pos{}, prev_pos{}, vel{ 0.0f, 0.0f };

    float bounceStrength = 1;
    int	bounceCooldown = 0;

    bool colliding = false;
    static constexpr float terminalVelocityMod = 12000.f;
    float terminalVelocity = 1.f;

    Ball(IVec2 screenSize);
    ~Ball();
    bool Collision(const Lines& line_array, const glm::vec2 prev_position);
    void Move(float dt);
    void Draw() const;
    void Reset(IVec2 screenSize);
    void activate();
    void deactivate();

private:
    struct rect rectDrawable;
};

#endif //LINHOP_BALL_HPP
