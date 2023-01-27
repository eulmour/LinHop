#ifndef ENGINE_DUMMYACTIVITY_H
#define ENGINE_DUMMYACTIVITY_H

#include "Engine.h"

namespace wuh {

class DummyActivity : public Activity {

public:
    DummyActivity() = default;
    ~DummyActivity() override = default;
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

#endif //ENGINE_DUMMYACTIVITY_H
