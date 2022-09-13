#ifndef LINHOP_TAIL_HPP
#define LINHOP_TAIL_HPP

#include "spige.h"
#include <list>

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

class Tail {

public:

    Tail(struct line* line, float alpha);

    struct Line {
        Line(glm::vec2 a, glm::vec2 b);
        void update();

        glm::vec2 a_pos;
        glm::vec2 b_pos;
        unsigned int lifeTime = 50;
    };

    void push(glm::vec2 a, glm::vec2 b);
    void draw();
    void reset();

public:
    float alpha = 1.0f;

private:
    static constexpr std::size_t tailLength = 50;
    static constexpr float tailFuzz = 10.f;
    std::list<Line> tail;
    struct line* line;
};

#endif //LINHOP_TAIL_HPP
