#include "Ball.hpp"
#include "Utils.hpp"

extern float scroll;
extern "C" spige* spige_instance;

Ball::Ball() : rectDrawable{} {
    glm_vec2_copy((vec2) {
        static_cast<float>(spige_instance->width) / 2.f, static_cast<float>(spige_instance->height) / 2.f
    }, this->pos);
}

Ball::~Ball() {
    if (this->rectDrawable.state != STATE_OFF)
        this->deactivate();
}

void Ball::activate() {
    rect_load(&this->rectDrawable);
    rect_use_texture(&this->rectDrawable, texture_load("textures/circle.png"));
    std::memcpy(&this->rectDrawable.color, (vec4){ 1.f, 1.f, 1.f, 1.f }, sizeof(vec4));
    std::memcpy(&this->rectDrawable.scale, (vec2){ radius*2, radius*2 }, sizeof(vec2));
}

void Ball::deactivate() {
    rect_unload(&this->rectDrawable);
}

bool Ball::Collision(const Lines& line_array, const vec2 prev_position) {
    for (const Lines::Line& line : reverse(line_array.lines)) {

        int side = checkLineSides(const_cast<float*>(line.a_pos), const_cast<float*>(line.b_pos), pos);

        if (intersect(
            const_cast<float*>(prev_position),
            pos,
            const_cast<float*>(line.a_pos),
            const_cast<float*>(line.b_pos)
        ) && sign(side) == 1 && bounceCooldown == 0) {

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
    glm_vec2_copy(pos, prev_pos);

    /* Update position */
    vel[1] = std::min(500 + terminalVelocity / terminalVelocityMod, vel[1] + gravity);
    glm_vec2_add(pos, (vec2){vel[0]*dt, vel[1]*dt}, pos);

    if (bounceCooldown > 0)
        --bounceCooldown;
}

void Ball::Draw() const {
    rect_draw(&this->rectDrawable, (vec2){ pos[0] - diameter, pos[1] - scroll - radius });
}

void Ball::Reset() {
    glm_vec2_copy((vec2){
        static_cast<float>(spige_instance->width) / 2.f,
        static_cast<float>(spige_instance->height) / 2.f + 30.f
    },pos);

    glm_vec2_copy((vec2){ 0.f, 0.f}, vel);

    bounceStrength = 1;
    bounceCooldown = 0;
    terminalVelocity = 1;
}
