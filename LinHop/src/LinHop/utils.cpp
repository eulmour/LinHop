#include "utils.h"

bool onSegment(glm::vec2 p, glm::vec2 q, glm::vec2 r)
{
	if ((q.x <= std::max(p.x, r.x)) &&
		(q.x >= std::min(p.x, r.x)) &&
		(q.y <= std::max(p.y, r.y)) &&
		(q.y >= std::min(p.y, r.y)))
	{
		return true;
	}
	return false;
}

int orientation(glm::vec2 p, glm::vec2 q, glm::vec2 r)
{
	float val = ((q.y - p.y) * (r.x - q.x)) - ((q.x - p.x) * (r.y - q.y));

	if (val > 0)
	{
		return 1;
	}
	else if (val < 0)
	{
		return 2;
	}
	else
		return 0;
}

bool intersect(glm::vec2 a1_pos, glm::vec2 a2_pos, glm::vec2 b1_pos, glm::vec2 b2_pos)
{
	int o1 = orientation(a1_pos, a2_pos, b1_pos);
	int o2 = orientation(a1_pos, a2_pos, b2_pos);
	int o3 = orientation(b1_pos, b2_pos, a1_pos);
	int o4 = orientation(b1_pos, b2_pos, a2_pos);

	if ((o1 != o2) && (o3 != o4))
		return true;

	if ((o1 == 0) && onSegment(a1_pos, b1_pos, a2_pos))
		return true;

	if ((o2 == 0) && onSegment(a1_pos, b2_pos, a2_pos))
		return true;

	if ((o3 == 0) && onSegment(b1_pos, a1_pos, b2_pos))
		return true;

	if ((o4 == 0) && onSegment(b1_pos, a2_pos, b2_pos))
		return true;

	return false;
}

int sign(float value)
{
	if (value != 0)
		return value / abs(value);
	else
		return 1;
}

int checkLineSides(glm::vec2 a_pos, glm::vec2 b_pos, glm::vec2 ball)
{
	return (b_pos.x - a_pos.x) * (ball.y - a_pos.y) - (b_pos.y - a_pos.y) * (ball.x - a_pos.x);
}
