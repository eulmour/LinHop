#include "Sparks.hpp"
#include "Utils.hpp"

using namespace linhop::utils;
extern float scroll;

Sparks::~Sparks() {
    if (this->rectDrawable.state != STATE_OFF)
        this->deactivate();
}

void Sparks::draw()
{
    auto current = sparks.begin();
    auto end = sparks.end();

    while (current != end)
    {
        current->update();

        this->rectDrawable.rot = degrees(std::atan2(-current->vel[0], -current->vel[1]));
        this->rectDrawable.color[0] = current->color[0];
        this->rectDrawable.color[1] = current->color[1];
        this->rectDrawable.color[2] = current->color[2];
        this->rectDrawable.color[3] = (static_cast<float>(current->life) / sparkLife);

        glm_vec2_copy(&current->size[0], this->rectDrawable.scale);
        rect_draw(&this->rectDrawable, &glm::vec2{ current->pos[0], current->pos[1] - scroll }[0]);

        if (current->life == 0)
        {
            sparks.erase(current++);
            continue;
        }
        ++current;
    }
}

void Sparks::push(glm::vec2 position)
{
    for (unsigned int i = 0; i < sparkAmount; ++i)
    {
        sparks.push_front(Spark(position));
    }
}

void Sparks::activate() {
    rect_load(&this->rectDrawable);
    rect_use_texture(&this->rectDrawable, texture_load("textures/sparkle.png"));

    this->rectDrawable.color[0] = 1.f;
    this->rectDrawable.color[1] = 1.f;
    this->rectDrawable.color[2] = 1.f;
    this->rectDrawable.color[3] = 1.f;

    this->rectDrawable.scale[0] = 50.f;
    this->rectDrawable.scale[1] = 50.f;
}

void Sparks::deactivate() {
    rect_unload(&this->rectDrawable);
}

Sparks::Spark::Spark(glm::vec2 pos) :
    pos(pos),
    vel{ t_rand(-20.0f, 20.0f), t_rand(-30.0f, -10.0f) },
    size{ t_rand(1.0f, 5.0f), t_rand(1.0f, 5.0f) },
    color{t_rand(0.0f, 1.0f),
        t_rand(0.0f, 1.0f),
        t_rand(0.0f, 1.0f),
        t_rand(0.0f, 1.0f)}
{}

void Sparks::Spark::update()
{
    vel[0] -= vel[0] / 5;
    vel[1] = std::min(1.0f, vel[1] + sparkGravity);
    pos += vel;

    --life;
}