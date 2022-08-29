#include "Label.hpp"

Label::Label(struct text* drawable, const std::string& text, glm::vec2 pos) : text(text), drawable(drawable) {
    this->pos = pos;
    this->color = { 1.f, 1.f, 1.f, 1.f };
}

void Label::draw() {
    std::memcpy(this->drawable->color, &this->color[0], sizeof(glm::vec4));
    this->width = text_draw(this->drawable, text.c_str(), &this->pos[0]) - this->pos[0];
}

bool Label::isCollide(const glm::vec2 position) {
    return
        position[0] > pos[0]
        && position[0] < pos[0] + this->width
        && position[1] > pos[1]
        && position[1] < pos[1] + drawable->size;
}