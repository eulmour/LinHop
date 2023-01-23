#include "Label.hpp"
#include <cstring>
#include <utility>

Label::Label(std::string text, glm::vec2 pos) : text(std::move(text)) {
    this->pos = pos;
    this->color = { 1.f, 1.f, 1.f, 1.f };
}

void Label::draw(const wuh::Graphics& g, const wuh::Text& drawable) {
    this->width = drawable.draw(g, text.c_str(), &this->pos[0], this->color) - this->pos[0];
}

bool Label::isCollide(const wuh::Text& drawable, const glm::vec2 position) {
    return
        position[0] > pos[0]
        && position[0] < pos[0] + this->width
        && position[1] > pos[1]
        && position[1] < pos[1] + drawable.size;
}