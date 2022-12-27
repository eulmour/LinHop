#ifndef ENGINE_DEBUG_SCREEN_H
#define ENGINE_DEBUG_SCREEN_H

#include <memory>

#include "Engine.h"
#include "Object.h" 
#include <vector>
#include <algorithm>

struct LogActivity : public Scene {

    explicit LogActivity(Engine& e, std::string content);
    ~LogActivity() override = default;
    void resume(Engine& e) override;
    void suspend(Engine& e) override;
    void render(Engine& e) override;
    virtual const char* title() override { return "Log"; }

protected:

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

    struct ScrollArea : Object {

        ScrollArea(Vec2 pos) {}
        void draw(const Graphics& g) {}

    protected:
        float scroll{ 0.f }, deltaScroll{ 0.f }, originScroll{ 0.f };
    };

	struct MultilineText {

        MultilineText(std::shared_ptr<Text> d, std::string text, std::size_t max)
            : d(d)
        {
            this->set_text(text, max);
        }

        void set_text(std::string text, std::size_t max) {

            if (text.empty()) {
                return;
            }

            std::istringstream iss(text);
            std::string line;

            do {
				if ((char)iss.rdbuf()->sgetc() == '\n') {
                    this->lines.push_back(line);
                    line.clear();
                    iss.get();
                    continue;
                }

                std::string word;
                iss >> word;

                if (line.length() > 0 && line.length() + word.length() > max) {
                    this->lines.push_back(line);
                    line.clear();
                }

				if (word.length() > max) {
					this->lines.push_back(word.substr(0, max));
                    line.clear();
                    word = word.substr(max, word.size());
				}

                line += word + " ";

            } while (iss);

            if (!line.empty()) {
				this->lines.push_back(line);
            }
        }

        void draw(const Graphics& g, Vec2 pos, Color c) {
            std::size_t i{ 0 };
            for (const auto& text : this->lines) {
				this->d->draw(g, text.c_str(), &Vec2{pos[0], pos[1] + static_cast<float>(i) * line_height}[0], c);
				++i;
            }
        }

        unsigned height() {
            return this->lines.size() * line_height;
        }

        std::shared_ptr<Text> d;
		std::vector<std::string> lines;
    };

	struct Resources {
        Line separator;
        Rect header_rect;
        std::shared_ptr<Rect> button_rect{std::make_shared<Rect>()};
		std::shared_ptr<Text> d_default_text{ std::make_shared<Text>("fonts/OCRAEXT.TTF", LogActivity::text_size) };
        Button back_btn{ d_default_text, button_rect, "<- Back" };
        MultilineText ml_text{ d_default_text, "", 1 };
    };

    static constexpr float line_height{ 25.f };
    static constexpr float text_size{ 24.f };

    std::string content;
    std::unique_ptr<Resources> res;
    float scroll{ 0.f }, deltaScroll{ 0.f }, originScroll{ 0.f };
};

#endif // ENGINE_DEBUG_SCREEN_H
