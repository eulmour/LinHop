#ifndef LINHOP_LASERS_HPP
#define LINHOP_LASERS_HPP

#include "engine/engine.h"
#include <list>
#include "glm/vec2.hpp"

class Lasers {

public:
    explicit Lasers();
    ~Lasers();

    void trigger(const wuh::Graphics& g, float position);
    void draw(const wuh::Graphics& g, const wuh::Line& drawable);
    void activate();
    void deactivate();

    class Laser
    {
    public:
        glm::vec2 a, b;

        Laser(glm::vec2 a, glm::vec2 b);
    };

    unsigned int live_time = 0U;
    float pos{};

    static constexpr std::size_t lasers_lives = 240;

    std::list<Laser> lasers;
private:
    std::unique_ptr<wuh::Rect> rect_drawable;
};

#endif //LINHOP_LASERS_HPP
