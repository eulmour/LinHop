#include "Util.hpp"
#include <cmath>

namespace linhop::util {

static std::default_random_engine& getDefaultRandomEngine()
{
    static std::random_device r;
    static std::default_random_engine e1(r());
    return e1;
}

float rand(float min, float max) {
    std::uniform_real_distribution<float> uniform_dist_real(min, max);
    return uniform_dist_real(getDefaultRandomEngine());
}

int rand(int min, int max) {
    std::uniform_int_distribution<int> uniform_dist_int(min, max);
    return uniform_dist_int(getDefaultRandomEngine());
}

int modinvabs(int a, int b) {
    int r = a % b;
    return r < 0 ? r + b : r;
}

float radians(float degree) {
    return (degree * (3.14159265359f / 180.0f));
}

float degrees(float radian) {
    return radian * (180.0f / 3.14159265359f);
}

float mirror_angle(float original, float base) {
    float dif = 180.0f - base;
    dif = 180.0f - static_cast<float>(std::fmod(original + dif, 360));
    return original + dif * 2;
}

float dis_func(float x, float y) {
    return static_cast<float>(std::sqrt(std::pow(x, 2) + std::pow(y, 2)));
}

float distance(const glm::vec2 a, const glm::vec2 b) {
    return static_cast<float>(std::sqrt(std::pow(b[0] - a[0], 2) + std::pow(b[1] - a[1], 2)));
}

float stabilize(float value, float floor) {

    if (value > floor) {
        return value - floor;
    } else if (value < -floor) {
        return value + floor;
    }

    return 0;
}

bool onSegment(glm::vec2 p, const glm::vec2 q, glm::vec2 r) {
    return ((q[0] <= std::max(p[0], r[0])) &&
        (q[0] >= std::min(p[0], r[0])) &&
        (q[1] <= std::max(p[1], r[1])) &&
        (q[1] >= std::min(p[1], r[1])));
}

int orientation(const glm::vec2 p, const glm::vec2 q, const glm::vec2 r) {
    const float val = ((q[1] - p[1]) * (r[0] - q[0])) - ((q[0] - p[0]) * (r[1] - q[1]));

    if (val > 0) {
        return 1;
    } else if (val < 0) {
        return 2;
    } else {
        return 0;
    }
}

int checkLineSides(const glm::vec2 a_pos, const glm::vec2 b_pos, const glm::vec2 ball) {
    return static_cast<int>((b_pos[0] - a_pos[0]) * (ball[1] - a_pos[1]) - (b_pos[1] - a_pos[1]) * (ball[0] - a_pos[0]));
}

bool intersect(glm::vec2 a1_pos, glm::vec2 a2_pos, glm::vec2 b1_pos, glm::vec2 b2_pos) {

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

int sign(int value) {
    return value >= 0 ? 1 : -1;
}

Color randColor(float factor, float alpha) {
    return {
        std::min(static_cast<float>(std::rand() % 255) / 255 + factor, 1.0f),
        std::min(static_cast<float>(std::rand() % 255) / 255 + factor, 1.0f),
        std::min(static_cast<float>(std::rand() % 255) / 255 + factor, 1.0f),
        alpha
    };
}

} // end of namespace linhop::util
