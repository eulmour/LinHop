#include "Lines.hpp"
#include "Utils.hpp"

extern "C" spige* spige_instance;

extern float scroll;

Lines::Lines(struct line* lineDrawable) : lineDrawable(lineDrawable) {}

void Lines::Push(vec2 second, vec2 first, bool isCol /* = true */)
{
    vec4 newColor;
    randColor(newColor, 1.f);
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

            memcpy(lineDrawable->color, circle.color, sizeof(circle.color));
            line_draw(lineDrawable, (vec4){ old_x, old_y - scroll, new_x, new_y - scroll });

            old_x = new_x;
            old_y = new_y;
        }
    };

    for (const auto& line : lines)
    {
        /* No off-screen rendering */
        if (line.a_pos[1] - scroll > spige_instance->height && line.b_pos[1] - scroll > spige_instance->height)
            continue;

        memcpy(lineDrawable->color, line.color, sizeof(line.color));
        line_draw(lineDrawable, (vec4){ line.a_pos[0], line.a_pos[1] - scroll, line.b_pos[0], line.b_pos[1] - scroll });

        if (!line.bCollinear)
            drawCircle(line.circle[0]);

        drawCircle(line.circle[1]);
    }
}

void Lines::Reset()
{
    lines.clear();

    lines.emplace_back( /* First line */
        (vec2){ 0.f, static_cast<float>(spige_instance->height) },
        (vec2){ static_cast<float>(spige_instance->width), static_cast<float>(spige_instance->height) },
        (vec4){ 1.0f, 1.0f, 1.0f, 1.0f },
        false
    );
}

Lines::Line::Line(vec2 a_pos, vec2 b_pos, vec4 color, bool isCol /* = true */) :
        bCollinear(isCol),
        circle{ { a_pos, color }, { b_pos, color } }
{
    glm_vec4_copy(color, this->color);

    if (a_pos[0] < b_pos[0]) {
        glm_vec2_copy(a_pos, this->a_pos);
        glm_vec2_copy(b_pos, this->b_pos);
    } else {
        glm_vec2_copy(b_pos, this->a_pos);
        glm_vec2_copy(a_pos, this->b_pos);
    }
}

Lines::Circle::Circle() {
    glm_vec2_copy(pos, (vec2){0.f, 0.f});
}

Lines::Circle::Circle(vec2 pos, vec4 color) {
    glm_vec2_copy(pos, this->pos);
    glm_vec4_copy(color, this->color);
}
