#ifndef NATIVEACTIVITY_LABEL_HPP
#define NATIVEACTIVITY_LABEL_HPP

#include "spige.h"
#include <string>

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

class Label {

public:
    Label(struct text* drawable, const std::string& text, glm::vec2 pos);
    void draw();
    Label& setText(std::string newText) { this->text = std::move(newText); return *this; }
    Label& setPos(glm::vec2 newPos) { this->pos = newPos; return *this; }
    Label& setColor(glm::vec4 newColor) { this->color = newColor; return *this; }
    bool isCollide(const glm::vec2 position);

//    void activate() { text_load(this->drawable); }
//    void deactivate() { text_unload(this->drawable); }
private:
    std::string text = {0};
    struct text* drawable = nullptr;
    glm::vec2 pos {1.f};
    glm::vec4 color {1.f};
    float width = 0.f;
};

#endif //NATIVEACTIVITY_LABEL_HPP
