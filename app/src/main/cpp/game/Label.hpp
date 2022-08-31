#ifndef NATIVEACTIVITY_LABEL_HPP
#define NATIVEACTIVITY_LABEL_HPP

#include "spige.h"
#include <string>
#include "GameObject.hpp"

class Label : public IGameObject {

public:
    Label(struct text* drawable, std::string text, glm::vec2 pos);
    void draw() override;
    Label& setText(std::string newText) { this->text = std::move(newText); return *this; }
    Label& setPos(glm::vec2 newPos) override { this->pos = newPos; return *this; }
    Label& setColor(glm::vec4 newColor) override { this->color = newColor; return *this; }
    bool isCollide(const glm::vec2 position);

    void activate() override { /* text_load(this->drawable); */ }
    void deactivate() override { /* text_unload(this->drawable); */ }

private:
    std::string text = {0};
    struct text* drawable = nullptr;
    float width = 0.f;
};

#endif //NATIVEACTIVITY_LABEL_HPP
