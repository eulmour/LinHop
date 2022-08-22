#include "Ball.hpp"
#include "Utils.hpp"

extern float scroll;
extern "C" spige* spige_instance;

Ball::Ball() : rectDrawable{} {

    this->pos = {
        static_cast<float>(spige_instance->width) / 2.f, static_cast<float>(spige_instance->height) / 2.f
    };
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

bool Ball::Collision(const Lines& line_array, const glm::vec2 prev_position) {
    for (const Lines::Line& line : reverse(line_array.lines)) {

        int side = checkLineSides(line.a_pos, line.b_pos, pos);

        if (intersect(
            prev_position, pos, line.a_pos, line.b_pos) && sign(side) == 1 && bounceCooldown == 0) {

            float angle = std::atan2(line.b_pos[1] - line.a_pos[1], line.b_pos[0] - line.a_pos[0]);
            float normal = angle - 3.1415926f * 0.5f;
            float mirrored = mirror_angle(degrees(std::atan2(-vel[1], -vel[0])), degrees(normal));
            float bounce_angle = radians(static_cast<float>(std::fmod(mirrored, 360)));

            vel[0] = std::cos(bounce_angle) * (dis_func(vel[0], vel[1]) + 100);
            vel[1] = std::sin(bounce_angle) * (dis_func(vel[0], vel[1]) + 1);
            vel[1] -= 300 * bounceStrength;

            bounceCooldown = 3;
            return true;
        }
    }

    return false;
}

void Ball::Move(float dt) {
    this->prev_pos = this->pos;

    /* Update position */
    vel.y = std::min(500 + terminalVelocity / terminalVelocityMod, vel[1] + gravity);
    this->pos += vel * dt;

    if (bounceCooldown > 0)
        --bounceCooldown;
}

void Ball::Draw() const {
    rect_draw(&this->rectDrawable, &glm::vec2{ pos[0] - diameter, pos[1] - scroll - radius }[0]);
}

void Ball::Reset() {

    this->pos = {
        static_cast<float>(spige_instance->width) / 2.f,
        static_cast<float>(spige_instance->height) / 2.f + 30.f
    };

    this->vel = { 0.f, 0.f };

    bounceStrength = 1;
    bounceCooldown = 0;
    terminalVelocity = 1;
}
