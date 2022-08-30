#include "Lines.hpp"
#include "Utils.hpp"

extern "C" spige* spige_instance;

extern float scroll;

Lines::Lines(struct line* lineDrawable) : lineDrawable(lineDrawable) {}

void Lines::Push(glm::vec2 second, glm::vec2 first, bool isCol /* = true */)
{
    glm::vec4 newColor;
    randColor(&newColor[0], 1.f);
    lines.emplace_back(first, second, newColor, isCol);
}

void Lines::Draw()
{
    const auto drawCircle = [this](const Circle& circle) {

        float old_x = circle.pos[0];
        float old_y = circle.pos[1] - circle.radius;

        // if (rand() % 500 == 0)
        // 	sparks.Push(pos);

        for (size_t i = 0; i <= circle.steps; ++i)
        {
            float new_x = circle.pos[0] + circle.radius * sinf(circle.angle * i);
            float new_y = circle.pos[1] + -circle.radius * cosf(circle.angle * i);

            std::memcpy(lineDrawable->color, &circle.color[0], sizeof(circle.color));
            line_draw(lineDrawable, &glm::vec4{ old_x, old_y - scroll, new_x, new_y - scroll }[0]);

            old_x = new_x;
            old_y = new_y;
        }
    };

    for (const auto& line : lines)
    {
        /* No off-screen rendering */
        if (line.a_pos[1] - scroll > spige_instance->height && line.b_pos[1] - scroll > spige_instance->height)
            continue;

        std::memcpy(lineDrawable->color, &line.color[0], sizeof(line.color));
        line_draw(lineDrawable, &glm::vec4{ line.a_pos[0], line.a_pos[1] - scroll, line.b_pos[0], line.b_pos[1] - scroll }[0]);

        if (!line.bCollinear)
            drawCircle(line.circle[0]);

        drawCircle(line.circle[1]);
    }
}

void Lines::Reset()
{
    lines.clear();

    lines.emplace_back( /* First line */
        glm::vec2{ 0.f, static_cast<float>(spige_instance->height) },
        glm::vec2{ static_cast<float>(spige_instance->width), static_cast<float>(spige_instance->height) },
        glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f },
        false
    );
}

Lines::Line::Line(glm::vec2 a_pos, glm::vec2 b_pos, glm::vec4 color, bool isCol /* = true */) :
        bCollinear(isCol),
        circle{ { a_pos, color }, { b_pos, color } }
{
    this->color = color;

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

Lines::Circle::Circle(glm::vec2 pos, glm::vec4 color) {
    this->pos = pos;
    this->color = color;
}
