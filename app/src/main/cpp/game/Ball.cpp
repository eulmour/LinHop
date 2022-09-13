#include "Ball.hpp"
#include "Utils.hpp"

extern float scroll;

using namespace linhop::utils;

Ball::Ball(IVec2 screenSize) : rectDrawable{} {
    this->pos = {static_cast<float>(screenSize[0]) / 2.f, static_cast<float>(screenSize[1]) / 2.f};
}

Ball::~Ball() {
    if (this->rectDrawable.state != STATE_OFF)
        this->deactivate();
}

void Ball::activate() {
    rect_load(&this->rectDrawable);
    rect_use_texture(&this->rectDrawable, texture_load("textures/circle.png"));

    this->rectDrawable.color[0] = 1.f;
    this->rectDrawable.color[1] = 1.f;
    this->rectDrawable.color[2] = 1.f;
    this->rectDrawable.color[3] = 1.f;

    this->rectDrawable.scale[0] = radius * 2;
    this->rectDrawable.scale[1] = radius * 2;
}

void Ball::deactivate() {
    rect_unload(&this->rectDrawable);
}

bool Ball::collision(const Lines& line_array, const glm::vec2 prev_position) {

    return std::any_of(
            line_array.lines.rbegin(),
            line_array.lines.rend(),
            [this, prev_position](const auto& line) {

        int side = checkLineSides(line.a_pos, line.b_pos, pos);

        if (intersect(
                prev_position, pos, line.a_pos, line.b_pos) && sign(side) == 1 && bounceCoolDown == 0) {

            float angle = std::atan2(line.b_pos[1] - line.a_pos[1], line.b_pos[0] - line.a_pos[0]);
            float normal = angle - 3.1415926f * 0.5f;
            float mirrored = mirror_angle(degrees(std::atan2(-vel[1], -vel[0])), degrees(normal));
            float bounce_angle = radians(static_cast<float>(std::fmod(mirrored, 360)));

            vel[0] = std::cos(bounce_angle) * (dis_func(vel[0], vel[1]) + 100);
            vel[1] = std::sin(bounce_angle) * (dis_func(vel[0], vel[1]) + 1);
            vel[1] -= 300 * bounceStrength;

            bounceCoolDown = 3;
            return true;
        } else {
            return false;
        }
    });
}

void Ball::move(float dt) {
    this->prev_pos = this->pos;

    /* Update position */
    vel.y = std::min(500 + terminalVelocity / terminalVelocityMod, vel[1] + gravity);
    this->pos += vel * dt;

    if (bounceCoolDown > 0)
        --bounceCoolDown;
}

void Ball::draw() const {
    rect_draw(&this->rectDrawable, &glm::vec2{ pos[0] - diameter, pos[1] - scroll - radius }[0]);
}

void Ball::reset(IVec2 screenSize) {

    this->pos = {
        static_cast<float>(screenSize[0]) / 2.f,
        static_cast<float>(screenSize[1]) / 2.f + 30.f
    };

    this->vel = { 0.f, 0.f };

    bounceStrength = 1;
    bounceCoolDown = 0;
    terminalVelocity = 1;
}
