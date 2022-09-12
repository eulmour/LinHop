#ifndef LINHOP_LAZERS_HPP
#define LINHOP_LAZERS_HPP

#include "spige.h"
#include <list>
#include "glm/vec2.hpp"
#include "GameObject.hpp"

//#define LAZERS_WIDTH (static_cast<float>(spige_instance->width) / 3.f)

class Lazers : IGameObject {

public:
    explicit Lazers(Engine& e, struct line* lineDrawable);
    ~Lazers();

    void Trigger(float position);
    void draw() override;
    void activate() override;
    void deactivate() override;

    class Lazer
    {
    public:
        glm::vec2 a, b;

        Lazer(glm::vec2 a, glm::vec2 b);
    };

    unsigned int liveTime = 0U;
    float areaWidth = 480.f / 3.f;
    float pos;

    static constexpr std::size_t lazersLives = 240;

    std::list<Lazer> lazers;
private:
    struct line* lineDrawable = nullptr;
    struct rect rectDrawable{};
};

#endif //LINHOP_LAZERS_HPP
