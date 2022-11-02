#ifndef LINHOP_LASERS_HPP
#define LINHOP_LASERS_HPP

#include "engine/engine.h"
#include <list>
#include "glm/vec2.hpp"

class Lasers {

public:
    explicit Lasers(IVec2 screen_size);
    ~Lasers();

    void trigger(float position);
    void draw(const Line& drawable);
    void activate(IVec2 screen_size);
    void deactivate();

    class Laser
    {
    public:
        glm::vec2 a, b;

        Laser(glm::vec2 a, glm::vec2 b);
    };

    unsigned int live_time = 0U;
    IVec2 screen_size{};
    float area_width = 480.f / 3.f;
    float pos{};

    static constexpr std::size_t lasers_lives = 240;

    std::list<Laser> lasers;
private:
    std::unique_ptr<Rect> rect_drawable;
};

#endif //LINHOP_LASERS_HPP
