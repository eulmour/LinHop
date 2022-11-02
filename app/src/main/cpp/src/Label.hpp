#ifndef LINHOP_LABEL_HPP
#define LINHOP_LABEL_HPP

#include "engine/engine.h"
#include <string>
#include "GameObject.hpp"

class Label : public IGameObject {

public:
    Label(struct text* drawable, std::string text, glm::vec2 pos);
    void draw() override;
    Label& setText(std::string newText) { this->text = std::move(newText); return *this; }
    Label& setPos(glm::vec2 newPos) { this->pos = newPos; return *this; }
    Label& setColor(Color newColor) { this->color = newColor; return *this; }
    bool isCollide(glm::vec2 position);

    void activate() override { /* text_load(this->drawable); */ }
    void deactivate() override { /* text_unload(this->drawable); */ }

private:
    std::string text = {0};
    struct text* drawable = nullptr;
    float width = 0.f;
};

#endif //LINHOP_LABEL_HPP
