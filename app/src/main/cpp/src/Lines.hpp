#ifndef LINHOP_LINES_HPP
#define LINHOP_LINES_HPP

#include "engine/engine.h"
#include <vector>

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

struct Circle {
    glm::vec2 pos{};
    wuh::Color color{};
    const unsigned int steps{static_cast<unsigned int>(3 + rand() % 7)};
    const float angle{3.1415926f * 2.0f / static_cast<float>(steps)};
    const float radius{20.0f};

    Circle();
    Circle(glm::vec2 pos, wuh::Color color);
    void draw(const wuh::Graphics& g, const wuh::Line& d, float width);
};

struct Lines {

    Lines();
    void Push(glm::vec2 second, glm::vec2 first, bool isCol = true);
    void draw(const wuh::Graphics& g);
    void Reset(const wuh::Graphics& g);

    struct Segment {
        bool collinear;
        glm::vec2 a_pos{};
        glm::vec2 b_pos{};
        wuh::Vec4 color;
        Circle circle[2];

        Segment(glm::vec2 a_pos, glm::vec2 b_pos, wuh::Color color, bool is_col = true);
    };

    void activate();
    void deactivate();

    std::vector<Segment> lines;

private:
    static constexpr float width = 5.f;
    std::unique_ptr<wuh::Line> d_segment;
    std::unique_ptr<wuh::Line> d_circle_segment;
};

#endif //LINHOP_LINES_HPP
