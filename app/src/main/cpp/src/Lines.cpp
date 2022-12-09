#include "Lines.hpp"
#include "Util.hpp"

using namespace linhop::util;

extern float scroll;

Lines::Lines()
    : d_segment(std::make_unique<MyLine>())
    , d_circle_segment(std::make_unique<Line>())
{}

void Lines::Push(glm::vec2 second, glm::vec2 first, bool isCol) {
    lines.emplace_back(first, second, randColor(), isCol);
}

void Lines::draw(const Graphics& g) {

    for (auto& line : lines) {
        // No off-screen rendering
        if (line.a_pos[1] - scroll > static_cast<float>(g.viewport()[1]) && line.b_pos[1] - scroll > static_cast<float>(g.viewport()[1]))
            continue;

        auto pos = Vec4{line.a_pos[0], line.a_pos[1] - scroll, line.b_pos[0], line.b_pos[1] - scroll };

        this->d_segment->use();
        this->d_segment->position(pos);
        this->d_segment->draw(g, &pos[0], line.color, this->width);

        if (!line.collinear)
            line.circle[0].draw(g, *this->d_circle_segment, this->width);

        line.circle[1].draw(g, *this->d_circle_segment, this->width);
    }
}

void Lines::Reset(const Graphics& g) {
    lines.clear();
}

void Lines::activate() {
    this->d_segment = std::make_unique<MyLine>();
}

void Lines::deactivate() {
    this->d_segment.reset();
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
    : pos{0.f, 0.f}
{}

Circle::Circle(glm::vec2 pos, Color color)
    : pos(pos)
    , color(color)
{}

void Circle::draw(const Graphics& g, const Line& d, float width) {

    float old_x = this->pos[0];
    float old_y = this->pos[1] - this->radius;

    for (size_t i = 0; i <= this->steps; ++i) {

        float new_x = this->pos[0] + this->radius * sinf(this->angle * i);
        float new_y = this->pos[1] + -this->radius * cosf(this->angle * i);

        const Vec4 segment_pos{old_x, old_y - scroll, new_x, new_y - scroll};

        d.use();
        d.draw(g, &segment_pos[0], this->color, width);

        old_x = new_x;
        old_y = new_y;
    }
}

