#pragma once
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

#define FRAND(min, max) (((float)rand() / (float)(RAND_MAX / (max - min)) + min))
#define FNRAND(ceil) ((float)rand() / (float)(RAND_MAX / (ceil)) - (ceil / 2))

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
class utils
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

float radians(float degree);
float degrees(float radian);
float mirror_angle(float original, float base);
float dis_func(float x, float y);
float normalize(float num, float amt);
bool onSegment(glm::vec2 p, glm::vec2 q, glm::vec2 r);
int orientation(glm::vec2 p, glm::vec2 q, glm::vec2 r);
bool intersect(glm::vec2 a1_pos, glm::vec2 a2_pos, glm::vec2 b1_pos, glm::vec2 b2_pos);
int sign(float value);
int checkLineSides(glm::vec2 a_pos, glm::vec2 b_pos, glm::vec2 ball);
