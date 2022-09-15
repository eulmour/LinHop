#include "Sparks.hpp"
#include "Utils.hpp"

using namespace linhop::utils;
extern float scroll;

Sparks::~Sparks() {
    if (this->rect_drawable.state != STATE_OFF)
        this->deactivate();
}

void Sparks::draw() {

    auto current = sparks.begin();
    auto end = sparks.end();

    while (current != end) {
        current->update();

        this->rect_drawable.rot = degrees(std::atan2(-current->vel[0], -current->vel[1]));
        this->rect_drawable.color[0] = current->color[0];
        this->rect_drawable.color[1] = current->color[1];
        this->rect_drawable.color[2] = current->color[2];
        this->rect_drawable.color[3] = (static_cast<float>(current->life) / spark_life);

        this->rect_drawable.scale = current->size;
        rect_draw(&this->rect_drawable, &glm::vec2{current->pos[0], current->pos[1] - scroll }[0]);

        if (current->life == 0) {
            sparks.erase(current++);
            continue;
        }

        ++current;
    }
}

void Sparks::push(glm::vec2 position) {

    for (unsigned int i = 0; i < spark_amount; ++i)
    {
        sparks.push_front(Spark(position));
    }
}

void Sparks::activate() {

    rect_load(&this->rect_drawable);
    rect_use_texture(&this->rect_drawable, texture_load("textures/sparkle.png"));

    this->rect_drawable.color[0] = 1.f;
    this->rect_drawable.color[1] = 1.f;
    this->rect_drawable.color[2] = 1.f;
    this->rect_drawable.color[3] = 1.f;
    this->rect_drawable.scale[0] = 50.f; // TODO ?
    this->rect_drawable.scale[1] = 50.f;
}

void Sparks::deactivate() {
    rect_unload(&this->rect_drawable);
}

Sparks::Spark::Spark(glm::vec2 pos) :
    pos(pos),
    vel{ t_rand(-20.0f, 20.0f), t_rand(-30.0f, -10.0f) },
    size{ t_rand(Sparks::Spark::min_size, Sparks::Spark::max_size),
          t_rand(Sparks::Spark::min_size, Sparks::Spark::max_size) },
    color{t_rand(0.0f, 1.0f),
        t_rand(0.0f, 1.0f),
        t_rand(0.0f, 1.0f),
        t_rand(0.0f, 1.0f)}
{}

void Sparks::Spark::update() {

    vel[0] -= vel[0] / 5;
    vel[1] = std::min(1.0f, vel[1] + spark_gravity);
    pos += vel;

    --life;
}