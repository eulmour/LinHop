#ifndef ENGINE_DEBUG_SCREEN_H
#define ENGINE_DEBUG_SCREEN_H

#include <memory>

#include "Engine.h"
#include "Object.h" 
#include <vector>
#include <algorithm>

struct DebugScreen : public Scene {

    explicit DebugScreen(Engine& e);
    ~DebugScreen() override = default;
    void resume(Engine& e) override;
    void suspend(Engine& e) override;
    void render(Engine& e) override;

private:

	struct Button : public Object {
        Button(std::shared_ptr<Text> d, std::shared_ptr<Rect> d_rect, std::string label)
            : d(d)
            , d_rect(d_rect)
            , text(label)
        {}
        void draw(const Graphics& g, Vec2 pos) {
            this->d_rect->use();
            this->d_rect->scale = Vec2{120.f, 40.f};
            this->d_rect->draw(g, &Vec2{pos[0] - 10.f, pos[1] - 10.f} [0] , Color{1.f, 1.f, 1.f, 0.5f});
			this->d->draw(g, this->text.c_str(), &pos[0] , Color{0.f, 0.f, 0.f, 1.f});
        }
    private:
        std::string text;
        std::shared_ptr<Text> d;
        std::shared_ptr<Rect> d_rect;
    };

	struct MultilineText {

        MultilineText(std::shared_ptr<Text> d, std::string text, std::size_t max)
            : d(d)
        {
            this->set_text(text, max);
        }

        void set_text(std::string text, std::size_t max) {

			std::size_t offset = 0;
            if (text.size() < 1) {
                return;
            }

			while (true) {

                std::size_t n = max + offset <= text.size()
                    ? text.rfind(' ', max + offset)
                    : text.rfind(' ', max + 2 * offset - text.size());

                if (n != std::string::npos) {
					this->lines.push_back(text.substr(offset, n - offset));
					if (n < offset) {
						break;
					}
					offset = n + 1;
                } else {
					this->lines.push_back(text.substr(offset, max));
					offset += max;
                }
			}
        }

        void draw(const Graphics& g, Vec2 pos, Color c) {

            std::for_each(this->lines.begin(), this->lines.end(), [&g, &pos, &c, this, i = 0](const std::string& text) mutable {
				this->d->draw(g, text.c_str(), &Vec2{pos[0], pos[1] + static_cast<float>(i) * 25.f}[0], c);
				++i;
			});
        }

        std::shared_ptr<Text> d;
		std::vector<std::string> lines;
    };

	struct Resources {
        Line separator;
        Rect header_rect;
        std::shared_ptr<Rect> button_rect{std::make_shared<Rect>()};
		std::shared_ptr<Text> d_default_text{ std::make_shared<Text>("fonts/OCRAEXT.TTF", DebugScreen::text_size) };
        Button back_btn{ d_default_text, button_rect, "<- Back" };
        MultilineText ml_text{ d_default_text, "", 1 };
    };

    static constexpr float text_size{ 24.f };
    std::unique_ptr<Resources> res;

    float scroll{ 0.f }, deltaScroll{ 0.f }, originScroll{ 0.f };
};

#endif // ENGINE_DEBUG_SCREEN_H
