#pragma once
//#include "../glm/glm.hpp"
//#include "../glm/gtc/matrix_transform.hpp"
//#include "../glm/gtc/type_ptr.hpp"

#include <random>
#include <type_traits>
#include "cglm/cglm.h"

#define CCAT(src, condition, a, b) ((condition) ? (src a) : (src b))
#define COUNT(array) (sizeof(array) / sizeof(array[0]))

namespace unused
{
    template <typename T>
    struct vec2
    {
        T x, y;

        vec2(T x, T y) : x(x), y(y)
        {

        }
    };

    template <typename T>
    struct vec3
    {
        T x, y, z;

        vec3(T x, T y, T z) : x(x), y(y), z(z)
        {

        }
    };
}

class Utils
{
public:
    template <typename T>
    static T normalize(T range_min, T range_max, T from)
    {
        return (from - range_min) / (range_max - range_min);
    }

    template <typename T>
    static T unnormalize(T range_min, T range_max, T normalized)
    {
        return normalized * (range_max - range_min) + range_min;
    }

    template <typename T>
    static T scale(T min_from, T max_from, T min_to, T max_to, T from)
    {
        return unnormalize(min_to, max_to, normalize(min_from, max_from, from));
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
};

template <typename T>
struct reversion_wrapper { T& iterable; };

template <typename T>
auto begin (reversion_wrapper<T> w) { return std::rbegin(w.iterable); }

template <typename T>
auto end (reversion_wrapper<T> w) { return std::rend(w.iterable); }

template <typename T>
reversion_wrapper<T> reverse (T&& iterable) { return { iterable }; }

float t_rand(float min, float max);
int t_rand(int min, int max);
int mod(int a, int b);
float radians(float degree);
float degrees(float radian);
float mirror_angle(float original, float base);
float dis_func(float x, float y);
float distance(const vec2 a, const vec2 b);
float normalize(float num, float amt);
bool onSegment(vec2 p, const vec2 q, vec2 r);
int orientation(const vec2 p, const vec2 q, const vec2 r);
bool intersect(vec2 a1_pos, vec2 a2_pos, vec2 b1_pos, vec2 b2_pos);
int sign(int value);
int checkLineSides(const vec2 a_pos, const vec2 b_pos, const vec2 ball);
void randColor(vec4 dest, float alpha, float factor = 0.5f);

/* increment / decrement support (C++11) */
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
    value = T(mod((static_cast<int>(value) - 1), static_cast<int>(T::END)));
    return value;
}
