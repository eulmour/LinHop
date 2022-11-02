#include "Lasers.hpp"
#include <cstring>
#include <string>

#include "glm/vec4.hpp"

#define LASERS_LINES_DEFAULT_COLOR Color{ 1.0f, 0.0f, 0.0f, 0.85f }
#define LASERS_INNER_DEFAULT_COLOR Color{ 1.0f, 0.0f, 0.0f, 0.5f }

Lasers::Lasers(IVec2 screen_size) :
    screen_size(screen_size),
    area_width(screen_size[0] / 3.f)
    {}

Lasers::~Lasers() = default;

void Lasers::trigger(float position)
{
    if (live_time == 0)
    {
        live_time = lasers_lives;

        lasers.emplace_back(
            glm::vec2{ position, 0.0f },
            glm::vec2{ position, static_cast<float>(this->screen_size[1]) });

        lasers.emplace_back(
            glm::vec2{position + area_width, 0.0f },
            glm::vec2{position + area_width, static_cast<float>(this->screen_size[1]) }
        );
    }
}

void Lasers::draw(const Line& drawable)
{
    if (lasers.size() > 0) {
        if (live_time < 1) {
            lasers.clear();
        } else {

            for (const Lasers::Laser& laser : lasers) {
                drawable.draw(&glm::vec4{laser.a[0], laser.a[1], laser.b[0], laser.b[1] }[0], LASERS_LINES_DEFAULT_COLOR);
            }

            if (live_time < 60) {

                this->rect_drawable->rot = 0.f;

                this->rect_drawable->scale[0] = lasers.back().a[0] - lasers.front().a[0];
                this->rect_drawable->scale[1] = static_cast<float>(this->screen_size[1]);
                this->rect_drawable->draw(&glm::vec2{lasers.front().a[0], lasers.front().a[1] }[0], LASERS_INNER_DEFAULT_COLOR);
            }

            --live_time;
        }
    }
}

void Lasers::activate(IVec2 screen_size) {
    this->rect_drawable = std::make_unique<Rect>(screen_size);
    this->rect_drawable->useTexture(texture_load("textures/pixel.png"));

    this->rect_drawable->scale[0] = 50.f;
    this->rect_drawable->scale[1] = 50.f;
}

void Lasers::deactivate() {
    this->rect_drawable.reset();
}

Lasers::Laser::Laser(glm::vec2 a, glm::vec2 b) : a(a), b(b) {}
