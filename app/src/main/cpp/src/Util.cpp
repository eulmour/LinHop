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

} // end of namespace linhop::util
