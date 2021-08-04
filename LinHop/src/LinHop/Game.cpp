#include "Game.h"

#include <list>
#include <string>
#include <cmath>
#include <time.h>

#include "GLCall.h"
#include "Resources.h"

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

#include "Utils.h"

#define TAIL_LENGTH 50
#define TAIL_FUZZ 10.0f
#define SPARK_LIFE 50
#define SPARK_GRAVITY 2.5f
#define SPARK_AMOUNT 6
#define RAND_LINES_DENSITY 200.0f /* Lower = higher */

#define SCROLL(point) { point.x, point.y - scroll}
#define CX (static_cast<float>(Info.width) / 2) /* Center x */
#define CY (static_cast<float>(Info.height) / 2) /* Center y */

glm::vec2		mousePos(240.0f, 720.0f); /* extern value */

namespace
{
	Renderer*	renderer;
	RectangleObject* ballRect;
	RectangleObject* sparkRect;
	LineObject* line;
	TextObject* text;

	glm::vec2	prevMousePos(240.0f, 720.0f);
	glm::vec2	lastClick;
	float		scroll = 0.0f;
	long		gameScore = 0L;
	float		bounceStrength = 1;
	int			bounceCooldown = 0;
	float		terminalVelocity = 300 + gameScore / 3000;
	bool		endGame = false;

	glm::vec4 randColor(float alpha, float factor = 0.5f)
	{
		float red = static_cast<float>(rand() % 255) / 255 + factor;
		float green = static_cast<float>(rand() % 255) / 255 + factor;
		float blue = static_cast<float>(rand() % 255) / 255 + factor;

		return { std::min(red, 1.0f), std::min(green, 1.0f), std::min(blue, 1.0f), alpha };
	}
} /* end of anonimous namespace */

class Sparks
{
public:
	Sparks() {}

	struct Spark
	{
		glm::vec2 pos, vel, size;
		glm::vec4 color{};
		unsigned int life = SPARK_LIFE;

		Spark(glm::vec2 pos) :
			pos(pos),
			color(1.0f),
			size{ FRAND(1.0f, 8.0f), FRAND(1.0f, 8.0f) },
			vel{FRAND(-20.0f, 20.0f), FRAND(-30.0f, -10.0f) } {}

		void Update()
		{
			vel.x -= vel.x / 5;
			vel.y = std::min(1.0f, vel.y + SPARK_GRAVITY);
			pos += vel;

			--life;
		}

		void Draw()
		{
			renderer->DrawRect(
				*sparkRect,
				SCROLL(pos),
				size,
				degrees(std::atan2(-vel.x, -vel.y)), /* rotation */
				{color.r, color.g, color.b, (static_cast<float>(life) / SPARK_LIFE)});
		}
	};

	void Push(glm::vec2 position)
	{
		for (unsigned int i = 0; i < SPARK_AMOUNT; ++i)
		{
			aSparks.push_front(Spark(position));
		}
	}

	void Draw()
	{
		auto current = aSparks.begin();
		auto end = aSparks.end();

		while (current != end)
		{
			current->Update();
			current->Draw();

			if (current->life == 0)
			{
				aSparks.erase(current++);
				continue;
			}
			++current;
		}
	}

	std::list<Spark> aSparks;

} sparks;

class Lines
{
public:

	Lines()
	{

	}

	void Push(glm::vec2 second, glm::vec2 first = lastClick, bool isCol = true)
	{
		lines.push_back(Line(first, second, randColor(1.0f), isCol));
	}

	void Draw()
	{
		for (const auto& line : lines)
		{
			line.Draw();
		}
	}

	void Reset()
	{
		lines.clear();

		lines.push_back(Lines::Line( /* First line */
			{ 0.0f, static_cast<float>(Info.height) },
			{ static_cast<float>(Info.width), static_cast<float>(Info.height) },
			{ 1.0f, 1.0f, 1.0f, 1.0f },
			false
		));
	}

	struct Circle
	{
		glm::vec2 pos;
		glm::vec4 color;
		unsigned int steps = 3 + rand() % 7;
		float angle = 3.1415926 * 2.0f / steps;
		float radius = 10.0f;

		Circle() : pos(0.0f, 0.0f) {}
		Circle(glm::vec2 pos, glm::vec4 color) :
			pos(pos), color(color) {}

		void Draw() const
		{
			float old_x = pos.x;
			float old_y = pos.y - radius;

			// if (rand() % 500 == 0)
			// 	sparks.Push(pos);

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
		bool bCollinear;
		glm::vec2 a_pos;
		glm::vec2 b_pos;
		glm::vec4 color;
		Circle circle[2];

		Line(glm::vec2 a_pos, glm::vec2 b_pos, glm::vec4 color, bool isCol = true) :
			bCollinear(isCol),
			color(color),
			circle{ { a_pos, color }, { b_pos, color } }
		{
			this->a_pos = a_pos.x < b_pos.x ? a_pos : b_pos;
			this->b_pos = a_pos.x < b_pos.x ? b_pos : a_pos;
		}

		void Draw() const
		{
			/* No off-screen rendering */
			if (a_pos.y - scroll > Info.height && b_pos.y - scroll > Info.height)
				return;

			renderer->DrawLine(*line, { a_pos.x, a_pos.y - scroll }, { b_pos.x, b_pos.y - scroll }, color);

			if (!bCollinear)
				circle[0].Draw();
			
			circle[1].Draw();
		}
	};

	std::vector<Line> lines;
} lines, rand_lines; /* end of class lines */

class Ball
{
public:

	float radius = 14.0f;
	float diameter = radius / 2;
	float gravity = 9.8f;
	glm::vec2 pos{}, prev_pos{}, vel{ 0.0f, 0.0f };

	Ball() : pos({240.0f, 360.0f }) {}

	void Collision(Lines& line_array, glm::vec2& prev_position)
	{
		for (const auto& line : reverse(line_array.lines))
		{
			int side = checkLineSides(line.a_pos, line.b_pos, pos);

			if (sign(side) == 1)
			{
				if (intersect(prev_position, pos, line.a_pos, line.b_pos) && bounceCooldown == 0)
				{
					float angle = std::atan2(line.b_pos.y - line.a_pos.y, line.b_pos.x - line.a_pos.x);
					float normal = angle - 3.1415926f * 0.5f;
					float mirrored = mirror_angle(degrees(std::atan2(-vel.y, -vel.x)), degrees(normal));
					float bounce_angle = radians(std::fmod(mirrored, 360));

					vel.x = std::cos(bounce_angle) * (dis_func(vel.x, vel.y) + 100);
					vel.y = std::sin(bounce_angle) * (dis_func(vel.x, vel.y) + 1);
					vel.y -= 300 * bounceStrength;

					sparks.Push(pos);
					bounceCooldown = 3;
					break;
				}	
			}
		}
	}

	void Move(float dt)
	{
		prev_pos = pos;

		/* Update position */
		vel.y = std::min(terminalVelocity, vel.y + gravity);
		pos += vel * dt;

		if (bounceCooldown > 0)
			--bounceCooldown;

		Collision(lines, prev_pos);
		Collision(rand_lines, prev_pos);
	}

	void Draw() const
	{
		renderer->DrawRect(*ballRect, { pos.x - diameter, pos.y - scroll - radius }, { radius, radius });
	}

	void Reset()
	{
		pos = { CX, CY };
		vel = { 0.0f, 0.0f };
	}

} ball;

class Tail
{

public:

	Tail(const float alpha) : alpha(alpha) {}
	struct Line
	{
		glm::vec2 a_pos;
		glm::vec2 b_pos;
		unsigned int lifeTime = TAIL_LENGTH;

		Line(glm::vec2 a, glm::vec2 b) : a_pos(a), b_pos(b) {}

		void Update()
		{
			float amount = (((TAIL_LENGTH - static_cast<float>(lifeTime)) * TAIL_FUZZ) / TAIL_LENGTH) / 2;

			a_pos.x += FRAND(-amount, amount);
			a_pos.y += FRAND(-amount, amount);
			b_pos.x += FRAND(-amount, amount);
			b_pos.y += FRAND(-amount, amount);

			--lifeTime;
		}

		void Draw(const Tail& tail_ref) const
		{
			renderer->DrawLine(
				*line,
				{ a_pos.x, a_pos.y - scroll },
				{ b_pos.x, b_pos.y - scroll },
				randColor((static_cast<float>(lifeTime) / TAIL_LENGTH) * tail_ref.alpha, 0.15f));
		}
	};

	void Push(glm::vec2& a, glm::vec2 b)
	{
		aTail.push_front(Line(a, b));
	}

	void Draw()
	{
		auto current = aTail.begin();
		auto end = aTail.end();

		while (current != end)
		{
			current->Draw(*this);
			current->Update();

			if (current->lifeTime == 0)
			{
				aTail.erase(current++);
				continue;
			}

			++current;
		}
	}

	void Reset()
	{
		aTail.clear();
	}

private:
	std::list<Line> aTail;
	float alpha = 1.0f;

} ball_tail{ 0.7f }, cursor_tail{ 0.15f };

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
	delete ballRect;
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
	Resources::LoadTexture("../res/textures/sparkle.png", "sparkle");

	/* Objects */
	renderer = new Renderer();

	ballRect = new RectangleObject(
		spriteShader,
		{ 0.0f, 0.0f },
		{ Info.width, Info.height },
		Resources::GetTexture("circle"));
	
	sparkRect = new RectangleObject(
		spriteShader,
		{ 0.0f, 0.0f },
		{ Info.width, Info.height },
		Resources::GetTexture("sparkle"));

	line = new LineObject(
		lineShader,
		{ 0.0f, 0.0f },
		{ 100.f, 100.0f },
		{ 1.0f, 1.0f, 1.0f },
		&Resources::GetTexture("pixel"));

	lines.Reset();

	text = new TextObject(
		"dummy",
		"../res/fonts/OCRAEXT.TTF",
		textShader,
		glm::vec2(0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		18
	);

	lastClick.x = static_cast<float>(Info.width / 2);
	lastClick.y = static_cast<float>(Info.height);

	srand(static_cast<unsigned int>(time(NULL)));
}

void LinHop::Message(int id)
{
	switch (id)
	{
	case GLFW_MOUSE_BUTTON_LEFT:

		if (!endGame)
		{
			lines.Push(mousePos);
			lastClick = mousePos;
		}

		break;

	case GLFW_KEY_R:

		if (endGame)
			ResetPlayer();

		break;

	case 666:
		mousePos.y += scroll;
		break;

	}
}

void LinHop::Update(float dt)
{

	gameScore = std::max(gameScore, -static_cast<long>((ball.pos.y - Info.height / 2)));
	bounceStrength = 1 + static_cast<float>(gameScore) / 8000;
	ball.gravity = 9.8f + static_cast<float>(gameScore) / 2000;

	ball.Move(dt);

	/* If ball reaches half of the screen then update scroll */
	if (ball.pos.y - (Info.height / 2 - 10) < scroll)
	{
		scroll += (ball.pos.y - (Info.height / 2 - 10) - scroll) / 10;
	}

	/* If game was over turn global scroll back */
	if (endGame)
	{
		scroll += (-scroll) / 100;
	}

	/* If the ball is out of screen then stop the game */
	if (ball.pos.x < 0 || ball.pos.x > Info.width || ball.pos.y - scroll > Info.height + ball.radius)
	{
		endGame = true;
	}

	/* Random platforms */
	static float last_place = RAND_LINES_DENSITY;

	if ((-scroll) - last_place > RAND_LINES_DENSITY)
	{
		if (rand() % 2 <= 1)
		{
			float base_y = scroll - 80.0f;
			float base_x = static_cast<float>(rand() % Info.width);

			struct line { glm::vec2 first; glm::vec2 second; }  new_line;
			new_line.first = { base_x, base_y };
			new_line.second = { base_x + ((rand() % Info.width) / 2) - CX / 4, base_y + ((rand() % Info.height) / 5) };

			if (dis_func(new_line.second.x - new_line.first.x, new_line.second.y - new_line.first.y) > 30.0f)
			{
				rand_lines.Push(new_line.second, new_line.first, false);
			}
		}

		last_place += RAND_LINES_DENSITY;
	}

	/* When player hits the line sparks will be created */


	/* Push for tail */
	ball_tail.Push(ball.pos, ball.prev_pos);
	cursor_tail.Push(mousePos, prevMousePos);
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
	ball.Draw();
	ball_tail.Draw();
	sparks.Draw();

	if (!endGame)
	{
		renderer->DrawLine(
			*line,
			{ lastClick.x, lastClick.y - scroll },
			{ mousePos.x, mousePos.y - scroll },
			{ 0.5f, 0.5f, 0.5f, 1.0f });


		renderer->DrawText(
			std::to_string(static_cast<int>(1 / dt)) + std::string(" fps"),
			*text,
			glm::vec2(Info.width - 70.0f, 5.0f),
			glm::vec3(0.6f, 0.8f, 1.0f),
			1
		);

		renderer->DrawText(
			"Score: " + std::to_string(gameScore),
			*text,
			glm::vec2(0.0f, 5.0f),
			glm::vec3(0.6f, 0.9f, 1.0f),
			1
		);
	}
	else
	{
		renderer->DrawText(
			"Score: " + std::to_string(gameScore),
			*text,
			glm::vec2(CX - 55, Info.height / 2 - 20),
			glm::vec3(0.6f, 0.9f, 1.0f),
			1
		);

		renderer->DrawText(
			"Press R",
			*text,
			glm::vec2(CX - 40, Info.height / 2),
			glm::vec3(1.0f, 0.8f, 0.6f),
			1
		);
	}

	lines.Draw();
	rand_lines.Draw();
	cursor_tail.Draw();

	prevMousePos = mousePos;
}

void LinHop::ResetPlayer()
{
	cursor_tail.Reset();

	mousePos = { Info.width / 2, Info.height };
	prevMousePos = lastClick = mousePos;
	scroll = 0.0f;
	gameScore = 0L;
	bounceStrength = 1;
	bounceCooldown = 0;
	terminalVelocity = 300 + gameScore / 3000;
	endGame = false;

	ball.Reset();
	lines.Reset();

	srand(static_cast<unsigned int>(time(NULL)));
}

void LinHop::Quit()
{
	Resources::Clear();
}
