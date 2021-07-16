#pragma once

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

class waveforms
{
private:
    static float _density;
public:
    static void set_density(float density)
    {
        _density = density;
    }
    static void tri(float& value, float range_min = -1.0f, float range_max = 1.0f)
    {
        if (value < range_min || value > range_max)
            _density = -_density;
        value += _density;
    }
};

