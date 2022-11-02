#ifndef LINHOP_BALL_HPP
#define LINHOP_BALL_HPP

#include "engine/engine.h"
#include "Lines.hpp"

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

class Ball {

public:
    static constexpr float radius{24.0f};
    static constexpr float diameter{radius / 2.f};
    static constexpr float terminal_velocity_mod{12000.f};

    float gravity{9.8f};
    float bounce_strength{1.f};
    int	bounce_cool_down{0};
    bool colliding{false};
    static constexpr float terminal_velocity{1.5f};
    glm::vec2 pos{}, prev_pos{}, vel{ 0.0f, 0.0f };

    explicit Ball(IVec2 screen_size);
    ~Ball();
    bool collision(const Lines& line_array, glm::vec2 prev_position);
    void move(float dt);
    void draw() const;
    void reset(IVec2 screen_size);
    void activate(IVec2 screen_size);
    void deactivate();

private:
    std::unique_ptr<Rect> rect_drawable;
};

#endif //LINHOP_BALL_HPP
