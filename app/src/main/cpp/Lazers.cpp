#include "Lazers.hpp"

extern "C" spige* spige_instance;

#define LAZERS_LINES_DEFAULT_COLOR (vec4){ 1.0f, 0.0f, 0.0f, 0.85f }
#define LAZERS_INNER_DEFAULT_COLOR (vec4){ 1.0f, 0.0f, 0.0f, 0.5f }

Lazers::Lazers(struct line* lineDrawable) : lineDrawable(lineDrawable) {}

Lazers::~Lazers() {
    if (this->rectDrawable.state != STATE_OFF)
        this->deactivate();
}

void Lazers::Trigger(float position)
{
    if (liveTime == 0)
    {
        liveTime = lazersLives;
        auto lazersWidth = LAZERS_WIDTH;

        lazers.emplace_back(
            (vec2){ position, 0.0f },
            (vec2){ position, static_cast<float>(spige_instance->height) });

        lazers.emplace_back(
            (vec2){ position + lazersWidth, 0.0f },
            (vec2){ position + lazersWidth, static_cast<float>(spige_instance->height) }
        );
    }
}

void Lazers::Draw()
{
    if (lazers.size() > 0) {
        if (liveTime < 1) {
            lazers.clear();
        } else {

            for (const Lazers::Lazer& lazer : lazers) {
                glm_vec4_copy(LAZERS_LINES_DEFAULT_COLOR, this->lineDrawable->color);
                line_draw(this->lineDrawable, (vec4){ lazer.a[0], lazer.a[1], lazer.b[0], lazer.b[1] });
            }

            if (liveTime < 60) {

                this->rectDrawable.rot = 0.f;
                glm_vec4_copy(LAZERS_INNER_DEFAULT_COLOR, this->rectDrawable.color);

                glm_vec2_copy((vec2){
                    lazers.back().a[0] - lazers.front().a[0],
                    static_cast<float>(spige_instance->height)
                }, this->rectDrawable.scale);

                rect_draw(&this->rectDrawable, (vec2){ lazers.front().a[0], lazers.front().a[1] });
            }

            --liveTime;
        }
    }
}

void Lazers::activate() {
    rect_load(&this->rectDrawable);
    rect_use_texture(&this->rectDrawable, texture_load("textures/pixel.png"));
    std::memcpy(&this->rectDrawable.color, (vec4){ 1.f, 1.f, 1.f, 1.f }, sizeof(vec4));
    std::memcpy(&this->rectDrawable.scale, (vec2){ 50.f, 50.f }, sizeof(vec2));
}

void Lazers::deactivate() {
    rect_unload(&this->rectDrawable);
}

Lazers::Lazer::Lazer(vec2 a, vec2 b) {
    glm_vec2_copy(a, this->a);
    glm_vec2_copy(b, this->b);
}
