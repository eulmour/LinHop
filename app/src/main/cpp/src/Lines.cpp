#include "Lines.hpp"
#include "Util.hpp"

using namespace linhop::util;

extern float scroll;

Lines::Lines()
    : line_drawable(std::make_unique<MyLine>())
{}

void Lines::Push(glm::vec2 second, glm::vec2 first, bool isCol) {
    lines.emplace_back(first, second, randColor(), isCol);
}

void Lines::draw(const Graphics& g, const Line& drawable) {

    (void)drawable;
    // const auto drawCircle = [&g, &drawable](const Circle& circle, float width) {

    //     float old_x = circle.pos[0];
    //     float old_y = circle.pos[1] - circle.radius;

    //     for (size_t i = 0; i <= circle.steps; ++i)
    //     {
    //         float new_x = circle.pos[0] + circle.radius * sinf(circle.angle * i);
    //         float new_y = circle.pos[1] + -circle.radius * cosf(circle.angle * i);

    //         // drawable.draw(g, &glm::vec4{old_x, old_y - scroll, new_x, new_y - scroll }[0], circle.color, width);
    //         // this->draw(g, &glm::vec4{old_x, old_y - scroll, new_x, new_y - scroll }[0], circle.color, width);

    //         old_x = new_x;
    //         old_y = new_y;
    //     }
    // };

    for (auto& line : lines)
    {
        // No off-screen rendering
        if (line.a_pos[1] - scroll > static_cast<float>(g.viewport()[1]) && line.b_pos[1] - scroll > static_cast<float>(g.viewport()[1]))
            continue;

        auto pos = Vec4{line.a_pos[0], line.a_pos[1] - scroll, line.b_pos[0], line.b_pos[1] - scroll };
        // this->line_drawable->draw_(g, &pos[0], line.color, this->width);

        this->line_drawable->use();
        this->line_drawable->position(pos);

        drawable.draw_(g, &pos[0], line.color, this->width);

        if (!line.collinear)
            line.circle[0].draw(g, this->width);
            // drawCircle(line.circle[0], this->width);

        line.circle[1].draw(g, this->width);
        // drawCircle(line.circle[1], this->width);
    }
}

void Lines::Reset(const Graphics& g)
{
    lines.clear();

    lines.emplace_back( // First line
        glm::vec2{ 0.f, static_cast<float>(g.viewport()[1]) },
        glm::vec2{static_cast<float>(g.viewport()[0]), static_cast<float>(g.viewport()[1]) },
        Color{ 1.0f, 1.0f, 1.0f, 1.0f },
        false
    );
}

void Lines::activate() {
    this->line_drawable = std::make_unique<MyLine>();
}

void Lines::deactivate() {
    this->line_drawable.reset();
}

Lines::Segment::Segment(glm::vec2 a_pos, glm::vec2 b_pos, Color color, bool is_col)
    : collinear(is_col)
    , color(color)
    , circle{ { a_pos, color }, { b_pos, color } }
{
    if (a_pos[0] < b_pos[0]) {
        this->a_pos = a_pos;
        this->b_pos = b_pos;
    } else {
        this->a_pos = b_pos;
        this->b_pos = a_pos;
    }
}

Circle::Circle()
    // : Line(Shader::Builder()
    //     .vertex(
    //         "layout(location = 0) in vec2 position;\n"

    //         "void main() {\n"
    //             "mat4 projection = ortho(0.0, u_res.x, u_res.y, 0.0, -1.0, 1.0);"
    //             "gl_Position = projection * vec4(position, 0.0, 1.0);\n"
    //         "}\n")
    //     .fragment(
    //         "uniform vec4 u_position;\n"

    //         "void main() {\n"
    //             "vec2 fragCoord = vec2(gl_FragCoord.x, u_res.y - gl_FragCoord.y);\n"
    //             "float lineLength = distance(u_position.xy, u_position.zw);\n"
    //             "vec2 vCenter = vec2((u_position.x + u_position.z) / 2.0, (u_position.y + u_position.w) / 2.0);\n"

    //             "if (distance(vCenter, fragCoord) > (lineLength/2.0) - 18.0) {\n"
    //                 "out_color = vec4(0.0, 0.0, 0.0, 0.0);\n"
    //             "} else {\n"
    //                 "out_color = u_color;\n"
    //             "}\n"
    //         "}")
    //     .mk_unique())
    // , u_position(std::make_pair<unsigned, Vec4>(Shader::uniform_location(this->shader->id(), "u_position"), {}))
{
    this->pos = { 0.f, 0.f };
}

Circle::Circle(glm::vec2 pos, Color color) : pos(pos), color(color) {}

void Circle::draw(const Graphics& g, float width) {

    float old_x = this->pos[0];
    float old_y = this->pos[1] - this->radius;

    for (size_t i = 0; i <= this->steps; ++i)
    {
        float new_x = this->pos[0] + this->radius * sinf(this->angle * i);
        float new_y = this->pos[1] + -this->radius * cosf(this->angle * i);

        // drawable.draw(g, &glm::vec4{old_x, old_y - scroll, new_x, new_y - scroll }[0], this->color, width);
        // this->draw_(g, &glm::vec4{old_x, old_y - scroll, new_x, new_y - scroll }[0], this->color, width);

        old_x = new_x;
        old_y = new_y;
    }
}

