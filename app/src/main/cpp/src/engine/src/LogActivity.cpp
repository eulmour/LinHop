#include "LogActivity.h"
#include <sstream>
#include <fstream>

LogActivity::LogActivity(Engine& e, std::string content)
	: content(std::move(content))
{}

void LogActivity::resume(Engine& e) {
	this->res = std::make_unique<LogActivity::Resources>(e.graphics);
	this->res->area.ml_text.set_text(this->content, 28);
	 //this->res->area.ml_text.set_text(
	 //	"Lorem Ipsum is simply dummy text of the "
	 //	"printing and typesetting industry. Lorem "
	 //	"Ipsum has been the industry's standard "
	 //	"dummy text ever since the 1500s, when "
	 //	"an unknown printer took a galley of "
	 //	"type and scrambled it to make a type "
	 //	"specimen book. It has survived not only "
	 //	"five centuries, but also the leap into "
	 //	"electronic typesetting, remaining essentially "
	 //	"unchanged. It was popularised in the 1960s "
	 //	"with the release of Letraset sheets "
	 //	"containing Lorem Ipsum passages, and more "
	 //	"recently with desktop publishing software "
	 //	"like Aldus PageMaker including versions of "
	 //	"Lorem Ipsum. Lorem Ipsum is simply dummy "
	 //	"text of the printing and typesetting "
	 //	"industry. Lorem Ipsum has been the "
	 //	"industry's standard dummy text ever since "
	 //	"the 1500s, when an unknown printer took a "
	 //	"galley of type and scrambled it to make a "
	 //	"type specimen book. It has survived not "
	 //	"only five centuries, but also the leap "
	 //	"into electronic typesetting, remaining "
	 //	"essentially unchanged. It was popularised "
	 //	"in the 1960s with the release of Letraset "
	 //	"sheets containing Lorem Ipsum passages, "
	 //	"and more recently with desktop publishing "
	 //	"software like Aldus PageMaker including "
	 //	"versions of Lorem Ipsum.", 28);

	//this->res->ml_text.set_text("Lorem Ipsum is simply dummy text of the printing and typesetting industry.", 28);
	//this->res->ml_text.set_text("LoremIpsumissimplydummytextofthe printingandtypesettingindustry.", 28);
}

void LogActivity::suspend(Engine& e) {
	this->res.reset();
}

void LogActivity::render(Engine& e) {

	// update
	if (res->back_btn.clicked(e.input.getPointerArray()[0])) { // TODO collision
		e.window->close();
	}
	if (e.input.isKeyUp(Input::Key::Back)) {
		e.popScene();
		return;
	}

	if (e.input.isKeyDown(Input::Key::Pointer)) {
		this->res->area.on_touch(e.input);
	}
	if (e.input.isKeyHold(Input::Key::Pointer)) {
		this->res->area.on_hold(e.input);
	}
	if (e.input.isKeyUp(Input::Key::Pointer)) {
		this->res->area.on_release(e.input);
	}

	// draw
    e.graphics.clear(Color{.5f, .5f, .5f, 1.f});

	res->area.draw(e.graphics, res->d_default_text);
	res->header_rect.scale = Vec2{ static_cast<float>(e.graphics.viewport()[0]), 59.f };
	res->header_rect.use();
	res->header_rect.draw(e.graphics, &Vec2{ 0.f, 0.f }[0] , Color{1.f, 1.f, 1.f, 0.8f});

	res->separator.use();
	res->separator.draw(e.graphics, &Vec4{
		0.f, 60.f, static_cast<float>(e.graphics.viewport()[0]), 60.f
		}[0], Color{0.f, 0.f, 0.f, 1.f}, 2.f);

	res->back_btn.draw(e.graphics, Vec2{20.f, 20.f});
}
