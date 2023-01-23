#ifndef ENGINE_EMPTYSCENE_H
#define ENGINE_EMPTYSCENE_H

#include "Engine.h"

namespace wuh {

class EmptyScene : public Scene {

public:
    EmptyScene() = default;
    ~EmptyScene() override = default;
    void resume(Engine& engine) override {
        (void)engine;
    };
    void suspend(Engine& engine) override {
        (void)engine;
    };
    void render(Engine& engine) override {
        (void)engine;
    };
    static Color getBackgroundColor() { return {0.f, 0.f, 0.f, 0.f}; }
};

} // end of namespace wuh

#endif //ENGINE_EMPTYSCENE_H
