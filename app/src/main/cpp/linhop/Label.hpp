#ifndef NATIVEACTIVITY_LABEL_HPP
#define NATIVEACTIVITY_LABEL_HPP

#include "spige.h"
#include <string>

class Label {

public:
    Label(struct text* drawable, const std::string& text, vec2 pos);
    void draw();
    Label& setText(std::string newText) { this->text = std::move(newText); return *this; }
    Label& setPos(vec2 newPos) { glm_vec2_copy(newPos, this->pos); return *this; }
    Label& setColor(vec4 newColor) { glm_vec4_copy(newColor, this->color); return *this; }
    bool isCollide(const vec2 position);

//    void activate() { text_load(this->drawable); }
//    void deactivate() { text_unload(this->drawable); }
private:
    std::string text = {0};
    struct text* drawable = nullptr;
    vec2 pos = {1.f};
    vec4 color = {1.f};
    float width = 0.f;
};

#endif //NATIVEACTIVITY_LABEL_HPP
