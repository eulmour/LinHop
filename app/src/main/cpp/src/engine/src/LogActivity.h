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

	struct MultilineText {
        MultilineText(std::string text, std::size_t max) {
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

        void draw(const Graphics& g, std::shared_ptr<Text> d, Vec2 pos, Color c) {
            std::size_t i{ 0 };
            for (const auto& text : this->lines) {
				d->draw(g, text.c_str(), &Vec2{pos[0], pos[1] + static_cast<float>(i) * line_height}[0], c);
				++i;
            }
        }

        float height() {
            return (static_cast<float>(this->lines.size()) * line_height) + line_height;
        }

		std::vector<std::string> lines;
    };

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

        ScrollArea(Vec2 pos, Vec2 size) : pos(pos), size(size) {}

        void draw(const Graphics& g, std::shared_ptr<Text> d) {

			this->ml_text.draw(g, d, Vec2{ 20.f, this->pos[1] + 20.f + scroll_ }, Color{ 0.f, 0.f, 0.f, 1.f });

            if (this->restoring) {

                float max_height = std::min(0.f, -(this->ml_text.height() - this->size[1]));
                constexpr static float error = -.1f;

				if (scroll_ >= error) {
                    scroll_ /= 1.2f;
				} else if (scroll_ <= max_height - error) {
                    scroll_ = ((scroll_ - max_height) / 1.2f) + max_height;
                } else {
					this->restoring = false;
                }
            }
        }

        void on_touch(Input& i) {
			this->origin_scroll = i.getPointerArray()[0][1];
			this->prev_scroll = scroll_;
        }

        void on_hold(Input& i) {

            const float new_scroll = (i.getPointerArray()[0][1] - this->origin_scroll) + this->prev_scroll;

			if (new_scroll >= 0.f) {
                scroll_ = new_scroll / 2.f;
                return;
            }

            float max_height = std::min(0.f, -(this->ml_text.height() - this->size[1]));
            if (new_scroll <= max_height) {
				scroll_ = ((new_scroll - max_height) / 2.f) + max_height;
                return;
            }

            scroll_ = new_scroll;
        }

        void on_release(Input& i) {
            if (scroll_ > 0.f || scroll_ < -this->ml_text.height() + this->size[1]) {
				this->restoring = true;
            }
        }

		MultilineText ml_text{ "", 1 };
    protected:
		Vec2 pos;
		Vec2 size;
		bool restoring{ false };
		float scroll_{ 0.f }, outer_scroll{ 0.f }, prev_scroll{ 0.f }, origin_scroll{ 0.f };
    };

	struct Resources {
        Resources(const Graphics& g)
            : area(
                Vec2{ 0.f, 60.f },
                Vec2{ static_cast<float>(g.viewport()[0]), static_cast<float>(g.viewport()[1]) - 60.f })
        {}

        Line separator;
        Rect header_rect;
        std::shared_ptr<Rect> button_rect{std::make_shared<Rect>()};
		std::shared_ptr<Text> d_default_text{ std::make_shared<Text>("fonts/OCRAEXT.TTF", LogActivity::text_size) };
        Button back_btn{ d_default_text, button_rect, "<- Back" };
        ScrollArea area;
    };

    static constexpr float line_height{ 25.f };
    static constexpr float text_size{ 24.f };

    std::string content;
    std::unique_ptr<Resources> res;
};

#endif // ENGINE_DEBUG_SCREEN_H
