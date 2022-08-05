#include "Label.hpp"

Label::Label(struct text* drawable, const std::string& text, vec2 pos) : text(text), drawable(drawable) {
    glm_vec2_copy(pos, this->pos);
    glm_vec4_copy((vec4) { 1.f, 1.f, 1.f, 1.f }, this->color);
}

void Label::draw() {
    glm_vec4_copy(this->color, this->drawable->color);
    this->width = text_draw(this->drawable, text.c_str(), this->pos) - this->pos[0];
}

bool Label::isCollide(const vec2 position) {
    return
        position[0] > pos[0]
        && position[0] < pos[0] + this->width
        && position[1] > pos[1]
        && position[1] < pos[1] + drawable->size;
}