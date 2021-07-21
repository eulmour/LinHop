#include "Game.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <cmath>

#include "GLCall.h"
#include "Resources.h"

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

#include "utils.h"

Renderer* renderer;
RectangleObject* rect;
LineObject* line;
TextObject* text;

glm::vec2 mousePos(240.0f, 720.0f);
glm::vec2 lastClick;
float scroll = 0.0f;

class Lines
{
public:

	Lines()
	{

	}

	void Push(glm::vec2 pos)
	{
		float red	= static_cast<float>(rand() % 255) / 255 + 0.5f;
		float green = static_cast<float>(rand() % 255) / 255 + 0.5f;
		float blue	= static_cast<float>(rand() % 255) / 255 + 0.5f;

		if (lastClick.x < pos.x)
			lines.push_back(Line(lastClick, pos, { red, green, blue }));
		else
			lines.push_back(Line(pos, lastClick, { red, green, blue }));
	}

	void Draw()
	{
		for (const auto& line : lines)
		{
			line.Draw();
		}
	}

	struct Circle
	{
		glm::vec2 pos;
		glm::vec3 color;
		unsigned int steps = 3 + rand() % 7;
		float angle = 3.1415926 * 2.0f / steps;
		float radius = 10.0f;

		Circle(glm::vec2 pos, glm::vec3 color) :
			pos(pos), color(color) {}

		void Draw() const
		{
			float old_x = pos.x;
			float old_y = pos.y - radius;

			for (size_t i = 0; i <= steps; ++i)
			{
				float new_x = pos.x + radius * sinf(angle * i);
				float new_y = pos.y + -radius * cosf(angle * i);

				renderer->DrawLine(*line, { old_x, old_y - scroll }, { new_x, new_y - scroll }, color);
				old_x = new_x;
				old_y = new_y;
			}
		}
	};

	struct Line
	{
		glm::vec2 a_pos;
		glm::vec2 b_pos;
		glm::vec3 color;
		Circle circle;

		Line(glm::vec2 a_pos, glm::vec2 b_pos, glm::vec3 color) :
			a_pos(a_pos), b_pos(b_pos), color(color), circle({ mousePos.x, mousePos.y + scroll }, color) {}

		void Draw() const
		{
			renderer->DrawLine(*line, { a_pos.x, a_pos.y - scroll }, { b_pos.x, b_pos.y - scroll }, color);
			circle.Draw();
		}
	};

	std::vector<Line> lines;
} lines;

class Ball
{
public:

	float radius = 20.0f;
	float diameter = radius / 2;
	float gravity = 9.8f / 1.5f;
	glm::vec2 pos{}, vel{ 0.0f, 1.0f };

	Ball() : pos({240.0f - diameter, 360.0f - diameter }) {}

	void Move(float dt)
	{
		glm::vec2 prev_pos = pos;

		/* Update position */
		vel.y += gravity;
		pos += vel * dt;

		/* check line collisions */
		if (pos.x < 0.0f || pos.x > static_cast<float>(Info.width))
		{
			// fail
		}

		for (const auto& line : lines.lines)
		{
			int side = checkLineSides(line.a_pos, line.b_pos, { pos.x, pos.y + radius });

			if (sign(side) == 1)
			{
				if (intersect(prev_pos, { pos.x, pos.y + radius }, line.a_pos, line.b_pos))
				{
					vel.y = -400.0f;
				}	
			}
		}
	}

	void Draw() const
	{
		renderer->DrawRect(*rect, { pos.x, pos.y - scroll }, { radius, radius });
	}

} ball;

LinHop::LinHop(unsigned int width, unsigned int height)
	: Lives(3),
	Level(1),
	State(LinHopState::GAME_MENU)
{
	Info.width = width;
	Info.height = height;
}

LinHop::~LinHop()
{
	delete renderer;
	//delete ball;
	delete rect;
	delete line;
}

void LinHop::Init()
{
	/* Setting shaders */
	Shader& spriteShader	= Resources::LoadShader("../res/shaders/sprite.shader", "sprite");
	Shader& lineShader		= Resources::LoadShader("../res/shaders/line.shader", "line");
	Shader& textShader		= Resources::LoadShader("../res/shaders/text.shader", "text");

	/* Setting textures */
	Resources::LoadTexture("../res/textures/circle.png", "circle");
	Resources::LoadTexture("../res/textures/pixel.png", "pixel");

	/* Objects */
	renderer = new Renderer();

	rect = new RectangleObject(
		spriteShader,
		{ 0.0f, 0.0f },
		{ Info.width, Info.height },
		Resources::GetTexture("circle"));

	line = new LineObject(
		lineShader,
		{ 0.0f, 0.0f },
		{ 100.f, 100.0f },
		{ 1.0f, 1.0f, 1.0f },
		&Resources::GetTexture("pixel"));

	lines.lines.push_back(Lines::Line( /* First line */
		{ 0.0f, static_cast<float>(Info.height) },
		{ static_cast<float>(Info.width), static_cast<float>(Info.height) },
		{ 1.0f, 1.0f, 1.0f }
	));

	text = new TextObject(
		"hijk",
		"../res/fonts/OCRAEXT.TTF",
		textShader,
		glm::vec2(0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		18
	);

	lastClick.x = static_cast<float>(Info.width / 2);
	lastClick.y = static_cast<float>(Info.height);
}

void LinHop::Message(int id)
{
	switch (id)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		lines.Push({ mousePos.x, mousePos.y + scroll });
		lastClick = { mousePos.x, mousePos.y + scroll };
		break;
	case GLFW_KEY_R:
		ResetPlayer();
		break;
	}
}

void LinHop::Update(float dt)
{
	ball.Move(dt);

	if (ball.pos.y - (Info.height / 2) < scroll)
	{
		scroll += (ball.pos.y - (Info.height / 2) - scroll) / 10;
	}
}

void LinHop::ClearScreen(float dt)
{
	static float color[3] = { 0.0f, 0.1f, 0.2f };
	static float direction = 0.0005f * dt;

	if (color[0] > 0.2f || color[0] < 0.0f)
		direction = -direction;

	color[0] += -direction / 2;
	color[1] += direction / 3;
	color[2] += direction / 2;

	glClearColor(color[0], color[1], color[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void LinHop::Render(float dt)
{
	lines.Draw();

	renderer->DrawLine(
		*line,
		{ lastClick.x, lastClick.y - scroll },
		mousePos,
		{ 0.5f, 0.5f, 0.5f });

	ball.Draw();

	renderer->DrawText(
		std::to_string(static_cast<int>(1 / dt)) + std::string(" fps"),
		*text,
		glm::vec2(0.0f, 0.0f),
		glm::vec3(0.6f, 0.8f, 1.0f),
		1
	); 

	renderer->DrawText(
		"scroll: " + std::to_string(scroll),
		*text,
		glm::vec2(0.0f, 20.0f),
		glm::vec3(0.6f, 0.9f, 1.0f),
		1
	);

	renderer->DrawText(
		"what: " + std::to_string(ball.pos.y),
		*text,
		glm::vec2(0.0f, 40.0f),
		glm::vec3(0.6f, 0.9f, 1.0f),
		1
	);
}

void LinHop::ResetPlayer()
{
}

void LinHop::Quit()
{
	Resources::Clear();
}
