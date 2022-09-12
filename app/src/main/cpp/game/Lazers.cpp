#include "Lazers.hpp"
#include <cstring>
#include <string>

#include "glm/vec4.hpp"

extern "C" spige* spige_instance;

#define LAZERS_LINES_DEFAULT_COLOR glm::vec4{ 1.0f, 0.0f, 0.0f, 0.85f }
#define LAZERS_INNER_DEFAULT_COLOR glm::vec4{ 1.0f, 0.0f, 0.0f, 0.5f }

Lazers::Lazers(Engine& e, struct line* lineDrawable) :
        lineDrawable(lineDrawable), areaWidth(e.window->getLogicalSize()[0] / 3.f) {}

Lazers::~Lazers() {
    if (this->rectDrawable.state != STATE_OFF)
        this->deactivate();
}

void Lazers::Trigger(float position)
{
    if (liveTime == 0)
    {
        liveTime = lazersLives;
//        auto lazersWidth = LAZERS_WIDTH;

        lazers.emplace_back(
            glm::vec2{ position, 0.0f },
            glm::vec2{ position, static_cast<float>(spige_instance->height) });

        lazers.emplace_back(
            glm::vec2{position + areaWidth, 0.0f },
            glm::vec2{position + areaWidth, static_cast<float>(spige_instance->height) }
        );
    }
}

void Lazers::draw()
{
    if (lazers.size() > 0) {
        if (liveTime < 1) {
            lazers.clear();
        } else {

            for (const Lazers::Lazer& lazer : lazers) {
                std::memcpy(this->lineDrawable->color, &LAZERS_LINES_DEFAULT_COLOR[0], sizeof(glm::vec4));
                line_draw(this->lineDrawable, &glm::vec4{ lazer.a[0], lazer.a[1], lazer.b[0], lazer.b[1] }[0]);
            }

            if (liveTime < 60) {

                this->rectDrawable.rot = 0.f;

                std::memcpy(this->rectDrawable.color, &LAZERS_INNER_DEFAULT_COLOR[0], sizeof(glm::vec4));

                this->rectDrawable.scale[0] = lazers.back().a[0] - lazers.front().a[0];
                this->rectDrawable.scale[1] = static_cast<float>(spige_instance->height);

                rect_draw(&this->rectDrawable, &glm::vec2{ lazers.front().a[0], lazers.front().a[1] }[0]);
            }

            --liveTime;
        }
    }
}

void Lazers::activate() {
    rect_load(&this->rectDrawable);
    rect_use_texture(&this->rectDrawable, texture_load("textures/pixel.png"));

    this->rectDrawable.color[0] = 1.f;
    this->rectDrawable.color[1] = 1.f;
    this->rectDrawable.color[2] = 1.f;
    this->rectDrawable.color[3] = 1.f;

    this->rectDrawable.scale[0] = 50.f;
    this->rectDrawable.scale[1] = 50.f;
}

void Lazers::deactivate() {
    rect_unload(&this->rectDrawable);
}

Lazers::Lazer::Lazer(glm::vec2 a, glm::vec2 b) {
    this-> a = a;
    this-> b = b;
}
