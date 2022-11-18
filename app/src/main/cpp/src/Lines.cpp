#include "Lines.hpp"
#include "Util.hpp"

using namespace linhop::util;

extern float scroll;

Lines::Lines() {}

void Lines::Push(glm::vec2 second, glm::vec2 first, bool isCol /* = true */) {
    lines.emplace_back(first, second, randColor(), isCol);
}

void Lines::Draw(const Graphics& g, const Line& drawable) {

    const auto drawCircle = [&g, &drawable](const Circle& circle, float width) {

        float old_x = circle.pos[0];
        float old_y = circle.pos[1] - circle.radius;

        for (size_t i = 0; i <= circle.steps; ++i)
        {
            float new_x = circle.pos[0] + circle.radius * sinf(circle.angle * i);
            float new_y = circle.pos[1] + -circle.radius * cosf(circle.angle * i);

            drawable.draw(g, &glm::vec4{old_x, old_y - scroll, new_x, new_y - scroll }[0], circle.color, width);

            old_x = new_x;
            old_y = new_y;
        }
    };

    for (const auto& line : lines)
    {
        /* No off-screen rendering */
        if (line.a_pos[1] - scroll > static_cast<float>(g.viewport()[1]) && line.b_pos[1] - scroll > static_cast<float>(g.viewport()[1]))
            continue;

        auto pos = Vec4{line.a_pos[0], line.a_pos[1] - scroll, line.b_pos[0], line.b_pos[1] - scroll };
        drawable.draw(g, &pos[0], line.color, this->width);

        if (!line.collinear)
            drawCircle(line.circle[0], this->width);

        drawCircle(line.circle[1], this->width);
    }
}

void Lines::Reset(const Graphics& g)
{
    lines.clear();

    lines.emplace_back( /* First line */
        glm::vec2{ 0.f, static_cast<float>(g.viewport()[1]) },
        glm::vec2{static_cast<float>(g.viewport()[0]), static_cast<float>(g.viewport()[1]) },
        Color{ 1.0f, 1.0f, 1.0f, 1.0f },
        false
    );
}

void Lines::activate() {
    this->line_drawable = std::make_unique<Line>();
}

void Lines::deactivate() {
    this->line_drawable.reset();
}

Lines::Segment::Segment(glm::vec2 a_pos, glm::vec2 b_pos, Color color, bool is_col /* = true */)
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

Lines::Circle::Circle() {
    this->pos = { 0.f, 0.f };
}

Lines::Circle::Circle(glm::vec2 pos, Color color) : pos(pos), color(color) {}
