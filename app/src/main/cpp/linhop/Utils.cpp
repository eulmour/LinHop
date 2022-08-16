#include "Utils.hpp"

static std::default_random_engine& getDefaultRandomEngine() 
{
    static std::random_device r;
    static std::default_random_engine e1(r());
    return e1;
}

float t_rand(float min, float max)
{
    std::uniform_real_distribution<float> uniform_dist_real(min, max);
    return uniform_dist_real(getDefaultRandomEngine());
}

int t_rand(int min, int max)
{
    std::uniform_int_distribution<int> uniform_dist_int(min, max);
    return uniform_dist_int(getDefaultRandomEngine());
}

int mod(int a, int b)
{
	int r = a % b;
	return r < 0 ? r + b : r;
}

float radians(float degree)
{
	return (degree * (3.14159265359f / 180));
}

float degrees(float radian)
{
	return radian * (180 / 3.14159265359f);
}

float mirror_angle(float original, float base)
{
	float dif = 180 - base;
	dif = 180 - static_cast<float>(std::fmod(original + dif, 360));

	return original + dif * 2;
}

float dis_func(float x, float y)
{
	return std::sqrt(std::powf(x, 2) + std::powf(y, 2));
}

float distance(const vec2 a, const vec2 b) {
	return static_cast<float>(std::sqrt(std::pow(b[0] - a[0], 2) + std::pow(b[1] - a[1], 2)));
}

float normalize(float num, float amt)
{
	if (num > amt)
	{
		num -= amt;
	}
	else if (num < -amt)
	{
		num += amt;
	}
	else
	{
		num = 0;
	}

	return num;
}

bool onSegment(vec2 p, const vec2 q, vec2 r)
{
	if ((q[0] <= std::max(p[0], r[0])) &&
		(q[0] >= std::min(p[0], r[0])) &&
		(q[1] <= std::max(p[1], r[1])) &&
		(q[1] >= std::min(p[1], r[1])))
	{
		return true;
	}
	return false;
}

int orientation(const vec2 p, const vec2 q, const vec2 r)
{
	float val = ((q[1] - p[1]) * (r[0] - q[0])) - ((q[0] - p[0]) * (r[1] - q[1]));

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

bool intersect(vec2 a1_pos, vec2 a2_pos, vec2 b1_pos, vec2 b2_pos)
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

int sign(int value)
{
	if (value != 0)
		return value / abs(value);
	else
		return 1;
}

int checkLineSides(const vec2 a_pos, const vec2 b_pos, const vec2 ball)
{
	return (b_pos[0] - a_pos[0]) * (ball[1] - a_pos[1]) - (b_pos[1] - a_pos[1]) * (ball[0] - a_pos[0]);
}

void randColor(vec4 dest, float alpha, float factor /*= 0.5f*/)
{
	float red = static_cast<float>(rand() % 255) / 255 + factor;
	float green = static_cast<float>(rand() % 255) / 255 + factor;
	float blue = static_cast<float>(rand() % 255) / 255 + factor;

	dest[0] = std::min(red, 1.0f);
	dest[1] = std::min(green, 1.0f);
	dest[2] = std::min(blue, 1.0f);
	dest[3] = alpha;
}