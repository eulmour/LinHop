#include "Lasers.hpp"
#include <cstring>
#include <string>

#include "glm/vec4.hpp"

#define LASERS_LINES_DEFAULT_COLOR glm::vec4{ 1.0f, 0.0f, 0.0f, 0.85f }
#define LASERS_INNER_DEFAULT_COLOR glm::vec4{ 1.0f, 0.0f, 0.0f, 0.5f }

Lasers::Lasers(Engine& e, struct line* lineDrawable) :
        screenSize(e.window->getLogicalSize()),
        areaWidth(e.window->getLogicalSize()[0] / 3.f),
        lineDrawable(lineDrawable)
        {}

Lasers::~Lasers() {
    if (this->rectDrawable.state != STATE_OFF)
        rect_unload(&this->rectDrawable);
}

void Lasers::trigger(float position)
{
    if (liveTime == 0)
    {
        liveTime = lasersLives;

        lasers.emplace_back(
            glm::vec2{ position, 0.0f },
            glm::vec2{ position, static_cast<float>(this->screenSize[1]) });

        lasers.emplace_back(
            glm::vec2{position + areaWidth, 0.0f },
            glm::vec2{position + areaWidth, static_cast<float>(this->screenSize[1]) }
        );
    }
}

void Lasers::draw()
{
    if (lasers.size() > 0) {
        if (liveTime < 1) {
            lasers.clear();
        } else {

            for (const Lasers::Laser& laser : lasers) {
                std::memcpy(this->lineDrawable->color, &LASERS_LINES_DEFAULT_COLOR[0], sizeof(glm::vec4));
                line_draw(this->lineDrawable, &glm::vec4{laser.a[0], laser.a[1], laser.b[0], laser.b[1] }[0]);
            }

            if (liveTime < 60) {

                this->rectDrawable.rot = 0.f;

                std::memcpy(this->rectDrawable.color, &LASERS_INNER_DEFAULT_COLOR[0], sizeof(glm::vec4));

                this->rectDrawable.scale[0] = lasers.back().a[0] - lasers.front().a[0];
                this->rectDrawable.scale[1] = static_cast<float>(this->screenSize[1]);

                rect_draw(&this->rectDrawable, &glm::vec2{lasers.front().a[0], lasers.front().a[1] }[0]);
            }

            --liveTime;
        }
    }
}

void Lasers::activate() {
    rect_load(&this->rectDrawable);
    rect_use_texture(&this->rectDrawable, texture_load("textures/pixel.png"));

    this->rectDrawable.color[0] = 1.f;
    this->rectDrawable.color[1] = 1.f;
    this->rectDrawable.color[2] = 1.f;
    this->rectDrawable.color[3] = 1.f;

    this->rectDrawable.scale[0] = 50.f;
    this->rectDrawable.scale[1] = 50.f;
}

void Lasers::deactivate() {
    rect_unload(&this->rectDrawable);
}

Lasers::Laser::Laser(glm::vec2 a, glm::vec2 b) : a(a), b(b) {}
