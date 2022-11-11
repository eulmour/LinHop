#ifndef LINHOP_LINES_HPP
#define LINHOP_LINES_HPP

#include "engine/engine.h"
#include <vector>

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

class Lines {
public:

    Lines();
    void Push(glm::vec2 second, glm::vec2 first, bool isCol = true);
    void Draw(const Graphics& g, const Line& drawable);
    void Reset(const Graphics& g);

    struct Circle
    {
        glm::vec2 pos{};
        Color color{};
        const unsigned int steps{static_cast<unsigned int>(3 + rand() % 7)};
        const float angle{3.1415926f * 2.0f / static_cast<float>(steps)};
        const float radius{20.0f};

        Circle();
        Circle(glm::vec2 pos, Color color);
    };

    struct Segment
    {
        bool collinear;
        glm::vec2 a_pos{};
        glm::vec2 b_pos{};
        Vec4 color;
        Circle circle[2];

        Segment(glm::vec2 a_pos, glm::vec2 b_pos, Color color, bool is_col = true);
    };

    static constexpr float width = 5.f;
    std::vector<Segment> lines;
};

#endif //LINHOP_LINES_HPP
