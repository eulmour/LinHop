#ifndef SPIGE_EMPTYSCENE_H
#define SPIGE_EMPTYSCENE_H

#include "Engine.h"

class EmptyScene : public Scene {

public:
    EmptyScene() = default;
    ~EmptyScene() override = default;
    void resume(Engine& engine) override {};
    void suspend(Engine& engine) override {};
    void update(Engine& engine) override {};
    void render(Engine& engine) override {};
    static Color getBackgroundColor() { return {0.f, 0.f, 0.f, 0.f}; }
};

#endif //SPIGE_EMPTYSCENE_H
