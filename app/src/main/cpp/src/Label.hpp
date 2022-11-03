#ifndef LINHOP_LABEL_HPP
#define LINHOP_LABEL_HPP

#include "engine/engine.h"
#include <string>
#include "GameObject.hpp"

class Label : public IGameObject {

public:
    Label(std::string text, glm::vec2 pos);
    void draw(const Graphics& g, const Text& drawable);
    Label& setText(std::string newText) { this->text = std::move(newText); return *this; }
    Label& setPos(glm::vec2 newPos) { this->pos = newPos; return *this; }
    Label& setColor(Color newColor) { this->color = newColor; return *this; }
    bool isCollide(const Text& drawable, glm::vec2 position);

    void activate() override { /* text_load(this->drawable); */ }
    void deactivate() override { /* text_unload(this->drawable); */ }

private:
    std::string text = {0};
    float width = 0.f;
};

#endif //LINHOP_LABEL_HPP
