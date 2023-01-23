#include "Ball.hpp"
#include "Util.hpp"

extern float scroll;

using namespace linhop;

Ball::Ball(const wuh::Graphics& g) : pos{static_cast<float>(g.size()[0]) / 2.f, static_cast<float>(g.size()[1]) / 2.f}
{}

Ball::~Ball() {}

void Ball::activate() {
    this->rect_drawable = std::make_unique<wuh::Rect>();
    this->rect_drawable->scale[0] = radius * 2;
    this->rect_drawable->scale[1] = radius * 2;
    this->rect_drawable->useTexture(wuh::texture_load_from_file("textures/circle.png"));
}

void Ball::deactivate() {
    this->rect_drawable.reset();
}

bool Ball::collision(const Lines& line_array, const glm::vec2 prev_position) {

    return std::any_of(
            line_array.lines.rbegin(),
            line_array.lines.rend(),
            [this, prev_position](const auto& line) {

        int side = util::checkLineSides(line.a_pos, line.b_pos, pos);

        if (util::intersect(
                prev_position, pos, line.a_pos, line.b_pos) && util::sign(side) == 1 && bounce_cool_down == 0) {

            float angle = std::atan2(line.b_pos[1] - line.a_pos[1], line.b_pos[0] - line.a_pos[0]);
            float normal = angle - 3.1415926f * 0.5f;
            float mirrored = util::mirror_angle(util::degrees(std::atan2(-vel[1], -vel[0])), util::degrees(normal));
            float bounce_angle = util::radians(static_cast<float>(std::fmod(mirrored, 360)));

            vel[0] = std::cos(bounce_angle) * (util::dis_func(vel[0], vel[1]) + 100);
            vel[1] = std::sin(bounce_angle) * (util::dis_func(vel[0], vel[1]) + 1);
            vel[1] -= 300 * bounce_strength;

            bounce_cool_down = 3;
            return true;
        } else {
            return false;
        }
    });
}

void Ball::move(float dt) {
    this->prev_pos = this->pos;

    /* Update position */
    vel.y = std::min(500 + terminal_velocity / terminal_velocity_mod, vel[1] + gravity);
    this->pos += vel * dt;

    if (bounce_cool_down > 0)
        --bounce_cool_down;
}

void Ball::draw(const wuh::Graphics& g) const {
    this->rect_drawable->draw(g, &glm::vec2{pos[0] - radius, pos[1] - scroll - radius}[0], wuh::Color{1.f, 1.f, 1.f, 1.f});
}

void Ball::reset(const wuh::Graphics& g) {

    this->pos = {
        static_cast<float>(g.size()[0]) / 2.f,
        static_cast<float>(g.size()[1]) / 2.f + 30.f
    };

    this->vel = { 0.f, 0.f };
    this->bounce_strength = 1;
    this->bounce_cool_down = 0;
}
