#ifndef ENGINE_DEBUG_SCREEN_H
#define ENGINE_DEBUG_SCREEN_H

#include <memory>

#include "Engine.h"
#include "Object.h" 

struct DebugScreen : public Scene {

    explicit DebugScreen(Engine& e);
    ~DebugScreen() override = default;
    void resume(Engine& e) override;
    void suspend(Engine& e) override;
    void render(Engine& e) override;

private:
    static constexpr float text_size{ 28.f };
    std::unique_ptr<Text> text;
    std::string message{};
    float scroll{ 0.f };
};

#endif // ENGINE_DEBUG_SCREEN_H
