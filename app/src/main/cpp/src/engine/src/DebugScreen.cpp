#include "DebugScreen.h"
#include <sstream>
#include <fstream>

DebugScreen::DebugScreen(Engine& e) {

	//std::ifstream f("log.txt");
	//std::stringstream buffer;
	//buffer << f.rdbuf();

	//this->message = std::string(buffer.str());
	//f.close();
}

void DebugScreen::resume(Engine& e) {
	this->res = std::make_unique<DebugScreen::Resources>();
}

void DebugScreen::suspend(Engine& e) {
	this->res.reset();
}

void DebugScreen::render(Engine& e) {

	//update
	if (res->back_btn.clicked(e.input.getPointerArray()[0])) { // TODO collision
		e.window->close();
	}

	// draw
    e.graphics.clear(Color{.5f, .5f, .5f, 1.f});

	res->separator.use();
	res->separator.draw(e.graphics, &Vec4{
		0.f, 60.f, static_cast<float>(e.graphics.viewport()[0]), 60.f
		}[0], Color{0.f, 0.f, 0.f, 1.f}, 2.f);

	res->back_btn.draw(e.graphics);
	res->d_default_text->draw(e.graphics, "Hello world", &Vec2{20.f, 80.f}[0] , Color{0.f, 0.f, 0.f, 1.f});
}

