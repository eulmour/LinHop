#include "Sparks.hpp"
#include "Utils.hpp"

extern float scroll;

Sparks::Sparks() {}

Sparks::~Sparks() {
    if (this->rectDrawable.state != STATE_OFF)
        this->deactivate();
}

void Sparks::Draw()
{
    auto current = aSparks.begin();
    auto end = aSparks.end();

    while (current != end)
    {
        current->Update();

        this->rectDrawable.rot = degrees(std::atan2(-current->vel[0], -current->vel[1]));
        this->rectDrawable.color[0] = current->color[0];
        this->rectDrawable.color[1] = current->color[1];
        this->rectDrawable.color[2] = current->color[2];
        this->rectDrawable.color[3] = (static_cast<float>(current->life) / sparkLife);

        glm_vec2_copy(&current->size[0], this->rectDrawable.scale);
        rect_draw(&this->rectDrawable, &glm::vec2{ current->pos[0], current->pos[1] - scroll }[0]);

        if (current->life == 0)
        {
            aSparks.erase(current++);
            continue;
        }
        ++current;
    }
}

void Sparks::Push(glm::vec2 position)
{
    for (unsigned int i = 0; i < sparkAmount; ++i)
    {
        aSparks.push_front(Spark(position));
    }
}

void Sparks::activate() {
    rect_load(&this->rectDrawable);
    rect_use_texture(&this->rectDrawable, texture_load("textures/sparkle.png"));
    glm_vec4_copy(&glm::vec4{ 1.f, 1.f, 1.f, 1.f }[0], this->rectDrawable.color);
    glm_vec2_copy(&glm::vec2{ 50.f, 50.f }[0], this->rectDrawable.scale);
}

void Sparks::deactivate() {
    rect_unload(&this->rectDrawable);
}

Sparks::Spark::Spark(glm::vec2 pos) {

    this->pos = pos;
    this->size = { t_rand(1.0f, 5.0f), t_rand(1.0f, 5.0f) };
    this->vel = { t_rand(-20.0f, 20.0f), t_rand(-30.0f, -10.0f) };
    this->color = {
        t_rand(0.0f, 1.0f),
        t_rand(0.0f, 1.0f),
        t_rand(0.0f, 1.0f),
        t_rand(0.0f, 1.0f)
    };
}

void Sparks::Spark::Update()
{
    vel[0] -= vel[0] / 5;
    vel[1] = std::min(1.0f, vel[1] + sparkGravity);
    pos += vel;
    // glm_vec2_add(pos, vel, pos);

    --life;
}