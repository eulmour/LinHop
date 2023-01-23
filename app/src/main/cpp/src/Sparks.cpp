#include "Sparks.hpp"
#include "Util.hpp"

using namespace linhop;
extern float scroll;

Sparks::~Sparks() = default;

void Sparks::draw(const wuh::Graphics& g) {

    auto current = sparks.begin();
    auto end = sparks.end();

    while (current != end) {
        current->update();

        this->rect_drawable->rot = util::degrees(std::atan2(-current->vel[0], -current->vel[1]));
        this->rect_drawable->scale = current->size;
        this->rect_drawable->draw(g, &glm::vec2{current->pos[0], current->pos[1] - scroll }[0], wuh::Color{
            current->color[0],
            current->color[1],
            current->color[2],
            static_cast<float>(current->life) / spark_life
        });

        if (current->life == 0) {
            sparks.erase(current++);
            continue;
        }

        ++current;
    }
}

void Sparks::push(glm::vec2 position) {

    for (unsigned int i = 0; i < spark_amount; ++i) {
        sparks.push_front(Spark(position));
    }
}

void Sparks::activate() {
    this->rect_drawable = std::make_unique<wuh::Rect>();
    this->rect_drawable->useTexture(wuh::texture_load_from_file("textures/sparkle.png"));
    this->rect_drawable->scale[0] = 50.f; // TODO ?
    this->rect_drawable->scale[1] = 50.f;
}

void Sparks::deactivate() {
    this->rect_drawable.reset();
}

Sparks::Spark::Spark(glm::vec2 pos) :
    pos(pos),
    vel{ util::rand(-20.0f, 20.0f), util::rand(-30.0f, -10.0f) },
    size{ util::rand(Sparks::Spark::min_size, Sparks::Spark::max_size),
          util::rand(Sparks::Spark::min_size, Sparks::Spark::max_size) },
    color{util::rand(0.0f, 1.0f),
        util::rand(0.0f, 1.0f),
        util::rand(0.0f, 1.0f),
        util::rand(0.0f, 1.0f)}
{}

void Sparks::Spark::update() {

    vel[0] -= vel[0] / 5;
    vel[1] = std::min(1.0f, vel[1] + spark_gravity);
    pos += vel;

    --life;
}