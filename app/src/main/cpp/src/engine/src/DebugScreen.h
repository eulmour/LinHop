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

	struct Button : public Object {
        Button(std::shared_ptr<Text> d, std::string label) : d(d), text(label) {}
        void draw(const Graphics& g) {
			this->d->draw(g, this->text.c_str(), &Vec2{20.f, 20.f} [0] , Color{0.f, 0.f, 0.f, 1.f});
        }
    private:
        std::string text;
        std::shared_ptr<Text> d;
    };

	struct Resources {
        Line separator;
        Button back_btn{ d_default_text, "<- Back" };
		std::shared_ptr<Text> d_default_text{ std::make_shared<Text>("fonts/OCRAEXT.TTF", DebugScreen::text_size) };
    };

    static constexpr float text_size{ 24.f };
    std::unique_ptr<Resources> res;

    float scroll{ 0.f };
	std::string message{};
};

#endif // ENGINE_DEBUG_SCREEN_H
