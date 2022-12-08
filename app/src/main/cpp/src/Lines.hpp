#ifndef LINHOP_LINES_HPP
#define LINHOP_LINES_HPP

#include "engine/engine.h"
#include <vector>

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

struct MyLine : public Line {
    MyLine()
        : Line(Shader::Builder()
/*            .vertex(
                "layout(location = 0) in vec2 position;\n"

                "void main() {\n"
                    "mat4 projection = ortho(0.0, u_res.x, u_res.y, 0.0, -1.0, 1.0);"
                    "gl_Position = projection * vec4(position, 0.0, 1.0);\n"
                "}\n")
            .fragment(
                "uniform vec4 u_position;\n"

                "void main() {\n"
                    "vec2 fragCoord = vec2(gl_FragCoord.x, u_res.y - gl_FragCoord.y);\n"
                    "float lineLength = distance(u_position.xy, u_position.zw);\n"
                    "vec2 vCenter = vec2((u_position.x + u_position.z) / 2.0, (u_position.y + u_position.w) / 2.0);\n"

                    "if (distance(vCenter, fragCoord) > (lineLength/2.0) - 26.0) {\n"
                        "out_color = vec4(0.0, 0.0, 0.0, 0.0);\n"
                    "} else {\n"
                        "out_color = u_color;\n"
                    "}\n"
                "}")
            .build())*/
            .from_file("shaders/line_segment.glsl"))
        , u_position(Shader::uniform_location(this->shader.id(), "u_position"))
    {}

    void position(Vec4 pos) {
        Shader::uniform_vec4(u_position, pos);
    }

    unsigned u_position;
};

struct Circle {
    glm::vec2 pos{};
    Color color{};
    const unsigned int steps{static_cast<unsigned int>(3 + rand() % 7)};
    const float angle{3.1415926f * 2.0f / static_cast<float>(steps)};
    const float radius{20.0f};

    Circle();
    Circle(glm::vec2 pos, Color color);
    void draw(const Graphics& g, const Line& d, float width);
};

struct Lines {

    Lines();
    void Push(glm::vec2 second, glm::vec2 first, bool isCol = true);
    void draw(const Graphics& g);
    void Reset(const Graphics& g);

    struct Segment {
        bool collinear;
        glm::vec2 a_pos{};
        glm::vec2 b_pos{};
        Vec4 color;
        Circle circle[2];

        Segment(glm::vec2 a_pos, glm::vec2 b_pos, Color color, bool is_col = true);
    };

    void activate();
    void deactivate();

    std::vector<Segment> lines;

private:
    static constexpr float width = 5.f;
    std::unique_ptr<MyLine> d_segment;
    std::unique_ptr<Line> d_circle_segment;
};

#endif //LINHOP_LINES_HPP
