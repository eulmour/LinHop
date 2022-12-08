#include "DebugScreen.h"
#include <sstream>
#include <fstream>

DebugScreen::DebugScreen(Engine& e) {}

void DebugScreen::resume(Engine & e) {

	this->text = std::make_unique<Text>("fonts/OCRAEXT.TTF", DebugScreen::text_size);

	std::ifstream f("log.txt");
	std::stringstream buffer;
	buffer << f.rdbuf();

	this->message = std::string(buffer.str());
	f.close();
}

void DebugScreen::suspend(Engine& e) {
	this->text.reset();
}

void DebugScreen::render(Engine& e) {}
