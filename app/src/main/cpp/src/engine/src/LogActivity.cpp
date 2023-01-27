#include "LogActivity.h"
#include <sstream>
#include <fstream>

namespace wuh {

LogActivity::LogActivity(Engine& e, std::string content)
	: content(std::move(content))
{
	(void)e;
}

void LogActivity::resume(Engine& e) {
	this->res = std::make_unique<LogActivity::Resources>(e.graphics);
	this->res->area.ml_text.set_text(this->content, 28);
}

void LogActivity::suspend(Engine& e) {
	(void)e;
	this->res.reset();
}

void LogActivity::render(Engine& e) {

	// update
	if (e.input.key_up(Input::Key::Back)) {
		e.pop_activity();
		return;
	}

	if (e.input.key_down(Input::Key::Pointer)) {
		this->res->area.on_touch(e.input);
	}
	if (e.input.key_hold(Input::Key::Pointer)) {
		this->res->area.on_hold(e.input);
	}
	if (e.input.key_up(Input::Key::Pointer)) {
		if (res->back_btn.clicked(e.input.pointers()[0])) {
			e.pop_activity();
			return;
		}
		if (res->clear_btn.clicked(e.input.pointers()[0])) {
			e.log().str(std::string());
			this->res->area.ml_text.set_text("", 0);
			this->content.clear(); // TODO
		}
		this->res->area.on_release(e.input);
	}

	// draw
    e.graphics.clear(Color{.5f, .5f, .5f, 1.f});

	res->area.draw(e.graphics, res->d_default_text);
	res->header_rect.scale = Vec2{ static_cast<float>(e.graphics.size()[0]), 59.f };
	res->header_rect.use();
	res->header_rect.draw(e.graphics, &Vec2{ 0.f, 0.f }[0] , Color{1.f, 1.f, 1.f, 0.8f});

	res->separator.use();
	res->separator.draw(e.graphics, &Vec4{
		0.f, 60.f, static_cast<float>(e.graphics.size()[0]), 60.f
		}[0], Color{0.f, 0.f, 0.f, 1.f}, 2.f);

	res->back_btn.pos(Vec2{ 20.f, 20.f });
	res->back_btn.draw(e.graphics);

	res->clear_btn.pos(Vec2{ e.graphics.size()[0] - 100.f, 20.f });
	res->clear_btn.draw(e.graphics);
}

} // end of namespace wuh