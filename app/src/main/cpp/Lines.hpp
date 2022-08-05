#ifndef LINHOP_LINES_HPP
#define LINHOP_LINES_HPP

#include "spige.h"
#include <vector>

class Lines {
public:

    Lines(struct line* lineDrawable);
    void Push(vec2 second, vec2 first, bool isCol = true);
    void Draw();
    void Reset();

    struct Circle
    {
        vec2 pos;
        vec4 color;
        unsigned int steps = 3 + rand() % 7;
        float angle = 3.1415926 * 2.0f / steps;
        float radius = 20.0f;

        Circle();
        Circle(vec2 pos, vec4 color);
    };

    struct Line
    {
        bool bCollinear;
        vec2 a_pos;
        vec2 b_pos;
        vec4 color;
        Circle circle[2];

        Line(vec2 a_pos, vec2 b_pos, vec4 color, bool isCol = true);
    };

    struct line* lineDrawable;
    std::vector<Line> lines;
};

#endif //LINHOP_LINES_HPP
