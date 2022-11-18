#ifndef LINHOP_LINES_HPP
#define LINHOP_LINES_HPP

#include "engine/engine.h"
#include <vector>

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

struct Lines {

    Lines();
    void Push(glm::vec2 second, glm::vec2 first, bool isCol = true);
    void Draw(const Graphics& g, const Line& drawable);
    void Reset(const Graphics& g);

    struct Circle {
        glm::vec2 pos{};
        Color color{};
        const unsigned int steps{static_cast<unsigned int>(3 + rand() % 7)};
        const float angle{3.1415926f * 2.0f / static_cast<float>(steps)};
        const float radius{20.0f};

        Circle();
        Circle(glm::vec2 pos, Color color);
    };

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
    std::unique_ptr<Line> line_drawable;
};

// struct Lines2 : public Line {

//     Lines2()
//         : Line(Shader::Builder()
//             .vertex(
//                 "#version 300 es\n"
//                 "precision mediump float;\n"

//                 SHADER_FN_ORTHO

//                 "layout(location = 0) in vec2 position;\n"
//                 "uniform vec2 resolution;\n"

//                 "void main() {\n"
//                     "mat4 projection = ortho(0.0, resolution.x, resolution.y, 0.0, -1.0, 1.0);"
//                     "gl_Position = projection * vec4(position, 0.0, 1.0);\n"
//                 "}\n")
//             .fragment(
//                 "#version 300 es\n"
//                 "precision mediump float;\n"

//                 "out vec4 fragColor;\n"
//                 "uniform vec2 resolution;\n"
//                 "uniform vec4 color;\n"
//                 "uniform vec4 position;\n"

//                 "void main() {\n"
//                     "vec2 fragCoord = vec2(gl_FragCoord.x, resolution.y - gl_FragCoord.y);\n"
//                     "float lineLength = distance(position.xy, position.zw);\n"
//                     "vec2 vCenter = vec2((position.x + position.z) / 2.0, (position.y + position.w) / 2.0);\n"

//                     "if (distance(vCenter, fragCoord) > (lineLength/2.0) - 18.0) {\n"
//                         "fragColor = vec4(0.0, 0.0, 0.0, 0.0);\n"
//                     "} else {\n"
//                         "fragColor = color;\n"
//                     "}\n"
//                 "}")
//             .build())
//         , resolution(std::make_pair<unsigned, Vec2>(Shader::uniform_location(this->shader->id(), "resolution"), {}))
//         , color(std::make_pair<unsigned, Vec4>(Shader::uniform_location(this->shader->id(), "color"), {}))
//         , position(std::make_pair<unsigned, Vec4>(Shader::uniform_location(this->shader->id(), "position"), {}))
//     {}

//     void push(glm::vec2 second, glm::vec2 first, bool isCol = true);
//     void draw(const Graphics& g, const Line& drawable);
//     void reset(const Graphics& g);

//     struct Circle {
//         glm::vec2 pos{};
//         Color color{};
//         const unsigned int steps{static_cast<unsigned int>(3 + rand() % 7)};
//         const float angle{3.1415926f * 2.0f / static_cast<float>(steps)};
//         const float radius{20.0f};

//         Circle();
//         Circle(glm::vec2 pos, Color color);
//     };

//     struct Segment {
//         bool collinear;
//         glm::vec2 a_pos{};
//         glm::vec2 b_pos{};
//         Vec4 color;
//         Circle circle[2];

//         Segment(glm::vec2 a_pos, glm::vec2 b_pos, Color color, bool is_col = true);
//     };

//     void activate();
//     void deactivate();

//     std::vector<Segment> lines;

//     void update() {
//         Shader::uniform_vec2(resolution.first, resolution.second);
//         Shader::uniform_vec4(color.first, color.second);
//         Shader::uniform_vec4(position.first, position.second);
//     }

//     std::pair<unsigned, Vec2> resolution;
//     std::pair<unsigned, Vec4> color;
//     std::pair<unsigned, Vec4> position;

// private:
//     static constexpr float width = 5.f;
// };
#endif //LINHOP_LINES_HPP
