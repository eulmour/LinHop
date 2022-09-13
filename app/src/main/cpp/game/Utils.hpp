#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <random>
#include <type_traits>
#include "spige.h"

#define CCAT(src, condition, a, b) ((condition) ? (src a) : (src b))
#define COUNT(array) (sizeof(array) / sizeof(array[0]))

namespace linhop::utils {

    float t_rand(float min, float max);
    int t_rand(int min, int max);
    int mod(int a, int b);
    float radians(float degree);
    float degrees(float radian);
    float mirror_angle(float original, float base);
    float dis_func(float x, float y);
    float distance(const glm::vec2 a, const glm::vec2 b);
    float normalize(float num, float amt);
    bool onSegment(glm::vec2 p, const glm::vec2 q, glm::vec2 r);
    int orientation(const glm::vec2 p, const glm::vec2 q, const glm::vec2 r);
    bool intersect(glm::vec2 a1_pos, glm::vec2 a2_pos, glm::vec2 b1_pos, glm::vec2 b2_pos);
    int sign(int value);
    int checkLineSides(const glm::vec2 a_pos, const glm::vec2 b_pos, const glm::vec2 ball);
    void randColor(float dest[4], float alpha, float factor = 0.5f);

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
}

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
    value = T(linhop::utils::mod((static_cast<int>(value) - 1), static_cast<int>(T::END)));
    return value;
}
