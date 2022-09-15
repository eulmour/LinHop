#ifndef LINHOP_LINES_HPP
#define LINHOP_LINES_HPP

#include "spige.h"
#include <vector>

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

class Lines {
public:

    Lines(Engine& e, struct line* lineDrawable);
    void Push(glm::vec2 second, glm::vec2 first, bool isCol = true);
    void Draw();
    void Reset();

    struct Circle
    {
        glm::vec2 pos{};
        glm::vec2 color{};
        const unsigned int steps{static_cast<unsigned int>(3 + rand() % 7)};
        const float angle{3.1415926f * 2.0f / static_cast<float>(steps)};
        const float radius{20.0f};

        Circle();
        Circle(glm::vec2 pos, glm::vec4 color);
    };

    struct Line
    {
        bool collinear;
        glm::vec2 a_pos{};
        glm::vec2 b_pos{};
        glm::vec4 color;
        Circle circle[2];

        Line(glm::vec2 a_pos, glm::vec2 b_pos, glm::vec4 color, bool is_col = true);
    };

    struct line* line_drawable;
    std::vector<Line> lines;
    IVec2 screen_size{};
};

#endif //LINHOP_LINES_HPP
