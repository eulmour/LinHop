#include "Tail.hpp"
#include "Utils.hpp"

using namespace linhop::utils;
extern float scroll;

Tail::Tail(struct line* line, const float alpha) : alpha(alpha), line(line) {}

void Tail::push(glm::vec2 a, glm::vec2 b)
{
    tail.push_front(Line(a, b));
}

void Tail::draw()
{
    auto current = tail.begin();
    auto end = tail.end();

    while (current != end)
    {
        randColor(&this->line->color[0], (static_cast<float>(current->life_time) / tail_length) * alpha, 0.15f);
        drawable.draw(this->line, &glm::vec4{ current->a_pos[0], current->a_pos[1] - scroll, current->b_pos[0], current->b_pos[1] - scroll }[0]);

        current->update();

        if (current->life_time == 0)
        {
            tail.erase(current++);
            continue;
        }

        ++current;
    }
}

void Tail::reset()
{
    tail.clear();
}

Tail::Line::Line(glm::vec2 a, glm::vec2 b) : a_pos(a), b_pos(b) {}

void Tail::Line::update()
{
    float amount = (((tail_length - static_cast<float>(life_time)) * tail_fuzz) / tail_length) / 2;

    a_pos[0] += t_rand(-amount, amount);
    a_pos[1] += t_rand(-amount, amount);
    b_pos[0] += t_rand(-amount, amount);
    b_pos[1] += t_rand(-amount, amount);

    --life_time;
}