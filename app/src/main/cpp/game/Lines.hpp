#ifndef LINHOP_LINES_HPP
#define LINHOP_LINES_HPP

#include "spige.h"
#include <vector>

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

class Lines {
public:

    Lines(struct line* lineDrawable);
    void Push(glm::vec2 second, glm::vec2 first, bool isCol = true);
    void Draw();
    void Reset();

    struct Circle
    {
        glm::vec2 pos;
        glm::vec2 color;
        unsigned int steps = 3 + rand() % 7;
        float angle = 3.1415926f * 2.0f / steps;
        float radius = 20.0f;

        Circle();
        Circle(glm::vec2 pos, glm::vec4 color);
    };

    struct Line
    {
        bool bCollinear;
        glm::vec2 a_pos;
        glm::vec2 b_pos;
        glm::vec4 color;
        Circle circle[2];

        Line(glm::vec2 a_pos, glm::vec2 b_pos, glm::vec4 color, bool isCol = true);
    };

    struct line* lineDrawable;
    std::vector<Line> lines;
};

#endif //LINHOP_LINES_HPP
