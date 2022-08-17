#include "Tail.hpp"
#include "Utils.hpp"

extern float scroll;

Tail::Tail(struct line* line, const float alpha) : alpha(alpha), line(line) {}

void Tail::Push(glm::vec2 a, glm::vec2 b)
{
    aTail.push_front(Line(a, b));
}

void Tail::Draw()
{
    auto current = aTail.begin();
    auto end = aTail.end();

    while (current != end)
    {
        randColor(&this->line->color[0], (static_cast<float>(current->lifeTime) / tailLength) * alpha, 0.15f);
        line_draw(this->line, &glm::vec4{ current->a_pos[0], current->a_pos[1] - scroll, current->b_pos[0], current->b_pos[1] - scroll }[0]);

        current->Update();

        if (current->lifeTime == 0)
        {
            aTail.erase(current++);
            continue;
        }

        ++current;
    }
}

void Tail::Reset()
{
    aTail.clear();
}

Tail::Line::Line(glm::vec2 a, glm::vec2 b) {
    this->a_pos = a;
    this->b_pos = b;
}

void Tail::Line::Update()
{
    float amount = (((tailLength - static_cast<float>(lifeTime)) * tailFuzz) / tailLength) / 2;

    a_pos[0] += t_rand(-amount, amount);
    a_pos[1] += t_rand(-amount, amount);
    b_pos[0] += t_rand(-amount, amount);
    b_pos[1] += t_rand(-amount, amount);

    --lifeTime;
}

//void Tail::Line::Draw(const Tail& tail_ref) const
//{
////    renderer->DrawLine(
////        *line,
////        { a_pos.x, a_pos.y - scroll },
////        { b_pos.x, b_pos.y - scroll },
////        randColor((static_cast<float>(lifeTime) / tailFuzz) * tail_ref.alpha, 0.15f));
//}