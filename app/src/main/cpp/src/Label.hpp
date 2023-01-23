#ifndef LINHOP_LABEL_HPP
#define LINHOP_LABEL_HPP

#include "engine/engine.h"
#include <string>
#include "GameObject.hpp"

class Label : public IGameObject {

public:
    Label(std::string text, glm::vec2 pos);
    void draw(const wuh::Graphics& g, const wuh::Text& drawable);
    Label& setText(std::string newText) { this->text = std::move(newText); return *this; }
    Label& setPos(glm::vec2 newPos) { this->pos = newPos; return *this; }
    Label& setColor(wuh::Color newColor) { this->color = newColor; return *this; }
    bool isCollide(const wuh::Text& drawable, glm::vec2 position);

    void activate() override {}
    void deactivate() override {}

private:
    std::string text = {0};
    float width = 0.f;
};

#endif //LINHOP_LABEL_HPP
