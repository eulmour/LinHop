#pragma once

#include <list>
#include <vector>

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

#define TAIL_LENGTH 50
#define TAIL_FUZZ 10.0f
#define SPARK_LIFE 50
#define SPARK_GRAVITY 2.5f
#define SPARK_AMOUNT 6
#define RAND_LINES_DENSITY 200.0f /* lower = higher */

inline class Sparks
{
public:
	Sparks();

	struct Spark
	{
		glm::vec2 pos, vel, size;
		glm::vec4 color{};
		unsigned int life = SPARK_LIFE;

		Spark(glm::vec2 pos);
		void Update();
		void Draw();
	};

	void Push(glm::vec2 position);
	void Draw();
    
	std::list<Spark> aSparks;

} sparks;

inline class Lines
{
public:

	Lines();

	void Push(glm::vec2 second, glm::vec2 first, bool isCol = true);

	void Draw();

	void Reset();

	struct Circle
	{
		glm::vec2 pos;
		glm::vec4 color;
		unsigned int steps = 3 + rand() % 7;
		float angle = 3.1415926 * 2.0f / steps;
		float radius = 10.0f;

		Circle();
		Circle(glm::vec2 pos, glm::vec4 color);

		void Draw() const;
	};

	struct Line
	{
		bool bCollinear;
		glm::vec2 a_pos;
		glm::vec2 b_pos;
		glm::vec4 color;
		Circle circle[2];

		Line(glm::vec2 a_pos, glm::vec2 b_pos, glm::vec4 color, bool isCol = true);

		void Draw() const;
	};

	std::vector<Line> lines;
} lines, rand_lines;

inline class Ball
{
public:

	float radius = 14.0f;
	float diameter = radius / 2;
	float gravity = 9.8f;
	glm::vec2 pos{}, prev_pos{}, vel{ 0.0f, 0.0f };

	Ball();
	void Collision(Lines& line_array, glm::vec2& prev_position);
	void Move(float dt);
	void Draw() const;
	void Reset();

} ball;

inline class Tail
{

public:

	Tail(const float alpha);

	struct Line
	{
		glm::vec2 a_pos;
		glm::vec2 b_pos;
		unsigned int lifeTime = TAIL_LENGTH;

		Line(glm::vec2 a, glm::vec2 b);

		void Update();

		void Draw(const Tail& tail_ref) const;
	};

	void Push(glm::vec2& a, glm::vec2 b);

	void Draw();

	void Reset();

public:
	float alpha = 1.0f;

private:
	std::list<Line> aTail;

} ball_tail{ 0.7f }, cursor_tail{ 0.08f };
