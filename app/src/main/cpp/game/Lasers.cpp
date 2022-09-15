#include "Lasers.hpp"
#include <cstring>
#include <string>

#include "glm/vec4.hpp"

#define LASERS_LINES_DEFAULT_COLOR Color{ 1.0f, 0.0f, 0.0f, 0.85f }
#define LASERS_INNER_DEFAULT_COLOR Color{ 1.0f, 0.0f, 0.0f, 0.5f }

Lasers::Lasers(Engine& e, struct line* lineDrawable) :
    screen_size(e.window->getLogicalSize()),
    area_width(e.window->getLogicalSize()[0] / 3.f),
    line_drawable(lineDrawable)
        {}

Lasers::~Lasers() {
    if (this->rect_drawable.state != STATE_OFF)
        rect_unload(&this->rect_drawable);
}

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

void Lasers::draw()
{
    if (lasers.size() > 0) {
        if (live_time < 1) {
            lasers.clear();
        } else {

            for (const Lasers::Laser& laser : lasers) {
                //std::memcpy(this->line_drawable->color, &LASERS_LINES_DEFAULT_COLOR[0], sizeof(glm::vec4));
                this->line_drawable->color = LASERS_LINES_DEFAULT_COLOR;
                line_draw(this->line_drawable, &glm::vec4{laser.a[0], laser.a[1], laser.b[0], laser.b[1] }[0]);
            }

            if (live_time < 60) {

                this->rect_drawable.rot = 0.f;

                //std::memcpy(this->rect_drawable.color, &LASERS_INNER_DEFAULT_COLOR[0], sizeof(glm::vec4));
                this->line_drawable->color = LASERS_INNER_DEFAULT_COLOR;

                this->rect_drawable.scale[0] = lasers.back().a[0] - lasers.front().a[0];
                this->rect_drawable.scale[1] = static_cast<float>(this->screen_size[1]);

                rect_draw(&this->rect_drawable, &glm::vec2{lasers.front().a[0], lasers.front().a[1] }[0]);
            }

            --live_time;
        }
    }
}

void Lasers::activate() {
    rect_load(&this->rect_drawable);
    rect_use_texture(&this->rect_drawable, texture_load("textures/pixel.png"));

    this->rect_drawable.color[0] = 1.f;
    this->rect_drawable.color[1] = 1.f;
    this->rect_drawable.color[2] = 1.f;
    this->rect_drawable.color[3] = 1.f;

    this->rect_drawable.scale[0] = 50.f;
    this->rect_drawable.scale[1] = 50.f;
}

void Lasers::deactivate() {
    rect_unload(&this->rect_drawable);
}

Lasers::Laser::Laser(glm::vec2 a, glm::vec2 b) : a(a), b(b) {}
