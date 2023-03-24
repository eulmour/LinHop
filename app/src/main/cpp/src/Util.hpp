#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <random>
#include <type_traits>
#include "engine/engine.h"

#define CCAT(src, condition, a, b) ((condition) ? (src a) : (src b))
#define COUNT(array) (sizeof(array) / sizeof(array[0]))

namespace linhop::util {

float rand(float min, float max);
int rand(int min, int max);
// float mirror_angle(float original, float base);
// float dis_func(float x, float y);
// float distance(const glm::vec2 a, const glm::vec2 b);
// bool onSegment(glm::vec2 p, const glm::vec2 q, glm::vec2 r);
// int orientation(const glm::vec2 p, const glm::vec2 q, const glm::vec2 r);
// bool intersect(glm::vec2 a1_pos, glm::vec2 a2_pos, glm::vec2 b1_pos, glm::vec2 b2_pos);
// int sign(int value);
// int checkLineSides(const glm::vec2 a_pos, const glm::vec2 b_pos, const glm::vec2 ball);
// wuh::Color randColor(float factor = 0.5f, float alpha = 1.0f);

template <typename T>
static T normalize(T range_min, T range_max, T from)
{
    return (from - range_min) / (range_max - range_min);
}

template <typename T>
static T denormalize(T range_min, T range_max, T normalized)
{
    return normalized * (range_max - range_min) + range_min;
}

template <typename T>
static T scale(T min_from, T max_from, T min_to, T max_to, T from)
{
    return denormalize(min_to, max_to, normalize(min_from, max_from, from));
}

template<typename T>
static T limit(T min, T max, T value)
{
    if (value > max)
        return max;

    if (value < min)
        return min;

    return value;
}

inline int modinvabs(int a, int b) {
    int r = a % b;
    return r < 0 ? r + b : r;
}

inline float radians(float degree) {
    return (degree * (3.14159265359f / 180.0f));
}

inline float degrees(float radian) {
    return radian * (180.0f / 3.14159265359f);
}

inline float dot(const glm::vec2 a, const glm::vec2 b) {
    return a.x*b.x + a.y*b.y;
}

inline float hypot2(const glm::vec2 a, const glm::vec2 b) {
    return dot(a - b, a - b);
}

inline glm::vec2 proj(const glm::vec2 a, const glm::vec2 b) {
    float k = dot(a, b) / dot(b, b);
    return {k * b.x, k * b.y};
}

inline float dis_to_line(const glm::vec2 a, const glm::vec2 b, const glm::vec2 point) {

    glm::vec2 ac = point - a;
    glm::vec2 ab = b - a;
    glm::vec2 d = proj(ac, ab) + a;
    glm::vec2 ad = d - a;

	float k;
	if (abs(ab.x) > abs(ab.y)) {
		k = ad.x / ab.x;
	} else {
		k = ad.y / ab.y;
	}

	if (k <= 0.0f) {
		return sqrt(hypot2(point, a));
	} else if (k >= 1.0) {
		return sqrt(hypot2(point, b));
	}

	return sqrt(hypot2(point, d));
}

inline float mirror_angle(float original, float base) {
    float dif = 180.0f - static_cast<float>(std::fmod(original + (180.0f - base), 360));
    return original + dif * 2;
}

inline float dis_func(float x, float y) {
    return static_cast<float>(std::sqrt(std::pow(x, 2) + std::pow(y, 2)));
}

inline float distance(const glm::vec2 a, const glm::vec2 b) {
    return static_cast<float>(std::sqrt(std::pow(b[0] - a[0], 2) + std::pow(b[1] - a[1], 2)));
}

inline float stabilize(float value, float floor) {

    if (value > floor) {
        return value - floor;
    } else if (value < -floor) {
        return value + floor;
    }

    return 0;
}

inline bool onSegment(glm::vec2 p, const glm::vec2 q, glm::vec2 r) {
    return ((q[0] <= std::max(p[0], r[0])) &&
        (q[0] >= std::min(p[0], r[0])) &&
        (q[1] <= std::max(p[1], r[1])) &&
        (q[1] >= std::min(p[1], r[1])));
}

inline int orientation(const glm::vec2 p, const glm::vec2 q, const glm::vec2 r) {
    const float val = ((q[1] - p[1]) * (r[0] - q[0])) - ((q[0] - p[0]) * (r[1] - q[1]));

    if (val > 0) {
        return 1;
    } else if (val < 0) {
        return 2;
    } else {
        return 0;
    }
}

inline int checkLineSides(const glm::vec2 a_pos, const glm::vec2 b_pos, const glm::vec2 ball) {
    return static_cast<int>((b_pos[0] - a_pos[0]) * (ball[1] - a_pos[1]) - (b_pos[1] - a_pos[1]) * (ball[0] - a_pos[0]));
}

inline bool intersect(glm::vec2 a1_pos, glm::vec2 a2_pos, glm::vec2 b1_pos, glm::vec2 b2_pos) {

    int o1 = orientation(a1_pos, a2_pos, b1_pos);
    int o2 = orientation(a1_pos, a2_pos, b2_pos);
    int o3 = orientation(b1_pos, b2_pos, a1_pos);
    int o4 = orientation(b1_pos, b2_pos, a2_pos);

    return ((o1 != o2) && (o3 != o4)) ||
        ((o1 == 0) && onSegment(a1_pos, b1_pos, a2_pos)) ||
        ((o2 == 0) && onSegment(a1_pos, b2_pos, a2_pos)) ||
        ((o3 == 0) && onSegment(b1_pos, a1_pos, b2_pos)) ||
        ((o4 == 0) && onSegment(b1_pos, a2_pos, b2_pos));
}

inline int sign(int value) {
    return value >= 0 ? 1 : -1;
}

inline wuh::Color randColor(float factor = 0.5f, float alpha = 1.0f) {
    return {
        std::min(static_cast<float>(std::rand() % 255) / 255 + factor, 1.0f),
        std::min(static_cast<float>(std::rand() % 255) / 255 + factor, 1.0f),
        std::min(static_cast<float>(std::rand() % 255) / 255 + factor, 1.0f),
        alpha
    };
}
} // end of namespace linhop::util

// reverse iterator support
template <typename T>
struct reversion_wrapper { T& iterable; };

template <typename T>
auto begin (reversion_wrapper<T> w) { return std::rbegin(w.iterable); }

template <typename T>
auto end (reversion_wrapper<T> w) { return std::rend(w.iterable); }

template <typename T>
reversion_wrapper<T> reverse (T&& iterable) { return { iterable }; }

// increment / decrement support (C++11)
template< typename T >
struct my_enum_is_unit_steppable { enum { value = false }; };

template< typename T >
typename std::enable_if< my_enum_is_unit_steppable< T >::value, T >::type
operator++(T& value)
{
    value = T((static_cast<int>(value) + 1) % static_cast<int>(T::END));
    return value;
}

template< typename T >
typename std::enable_if< my_enum_is_unit_steppable< T >::value, T >::type
operator--(T& value)
{
    value = T(linhop::util::modinvabs((static_cast<int>(value) - 1), static_cast<int>(T::END)));
    return value;
}
