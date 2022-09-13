#ifndef LINHOP_LASERS_HPP
#define LINHOP_LASERS_HPP

#include "spige.h"
#include <list>
#include "glm/vec2.hpp"
#include "GameObject.hpp"

class Lasers : IGameObject {

public:
    explicit Lasers(Engine& e, struct line* lineDrawable);
    ~Lasers() override;

    void trigger(float position);
    void draw() override;
    void activate() override;
    void deactivate() override;

    class Laser
    {
    public:
        glm::vec2 a, b;

        Laser(glm::vec2 a, glm::vec2 b);
    };

    unsigned int liveTime = 0U;
    IVec2 screenSize{};
    float areaWidth = 480.f / 3.f;
    float pos{};

    static constexpr std::size_t lasersLives = 240;

    std::list<Laser> lasers;
private:
    struct line* lineDrawable = nullptr;
    struct rect rectDrawable{};
};

#endif //LINHOP_LASERS_HPP
