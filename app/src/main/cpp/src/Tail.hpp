#ifndef LINHOP_TAIL_HPP
#define LINHOP_TAIL_HPP

#include "engine/engine.h"
#include <list>

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

class Tail {

public:

    Tail(float alpha);

    struct TailLine {
        TailLine(glm::vec2 a, glm::vec2 b);
        void update();

        glm::vec2 a_pos;
        glm::vec2 b_pos;
        unsigned int life_time = 50;
    };

    void push(glm::vec2 a, glm::vec2 b);
    void draw(const Graphics& g, const Line& drawable);
    void reset();

public:
    float alpha = 1.0f;

private:
    static constexpr std::size_t tail_length = 50;
    static constexpr float tail_fuzz = 10.f;
    std::list<TailLine> tail;
};

#endif //LINHOP_TAIL_HPP
