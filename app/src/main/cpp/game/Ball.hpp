#ifndef LINHOP_BALL_HPP
#define LINHOP_BALL_HPP

#include "spige.h"
#include "Lines.hpp"

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

class Ball {
public:

    float radius{18.0f};
    float diameter{radius / 2.f};
    float gravity{9.8f};
    glm::vec2 pos{}, prev_pos{}, vel{ 0.0f, 0.0f };

    float bounceStrength{1.f};
    int	bounceCoolDown{0};

    bool colliding{false};
    static constexpr float terminalVelocityMod{12000.f};
    float terminalVelocity{1.f};

    explicit Ball(IVec2 screenSize);
    ~Ball();
    bool collision(const Lines& line_array, glm::vec2 prev_position);
    void move(float dt);
    void draw() const;
    void reset(IVec2 screenSize);
    void activate();
    void deactivate();

private:
    struct rect rectDrawable;
};

#endif //LINHOP_BALL_HPP
