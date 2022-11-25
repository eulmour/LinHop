#include "Tail.hpp"
#include "Util.hpp"

using namespace linhop;
extern float scroll;

Tail::Tail(const float alpha) : alpha(alpha) {}

void Tail::push(glm::vec2 a, glm::vec2 b) {
    tail.push_front(TailLine(a, b));
}

void Tail::draw(const Graphics& g, const Line& drawable) {

    auto current = tail.begin();
    auto end = tail.end();

    while (current != end) {
        drawable.draw_(
            g,
            &glm::vec4{ current->a_pos[0], current->a_pos[1] - scroll, current->b_pos[0], current->b_pos[1] - scroll }[0],
            util::randColor((static_cast<float>(current->life_time) / tail_length) * alpha, 0.15f));

        current->update();

        if (current->life_time == 0) {
            tail.erase(current++);
            continue;
        }

        ++current;
    }
}

void Tail::reset() {
    tail.clear();
}

Tail::TailLine::TailLine(glm::vec2 a, glm::vec2 b) : a_pos(a), b_pos(b) {}

void Tail::TailLine::update() {
    
    float amount = (((tail_length - static_cast<float>(life_time)) * tail_fuzz) / tail_length) / 2;

    a_pos[0] += util::rand(-amount, amount);
    a_pos[1] += util::rand(-amount, amount);
    b_pos[0] += util::rand(-amount, amount);
    b_pos[1] += util::rand(-amount, amount);

    --life_time;
}