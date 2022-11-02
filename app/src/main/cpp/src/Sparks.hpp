#ifndef LINHOP_SPARKS_HPP
#define LINHOP_SPARKS_HPP

#include "engine/engine.h"
#include <list>

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

class Sparks {

public:
    Sparks() = default;
    ~Sparks();

    struct Spark {
        glm::vec2 pos, vel{0.f, 0.f};
        Vec2 size;
        glm::vec4 color{};
        unsigned int life{50};
        static constexpr float max_size{15.f};
        static constexpr float min_size{3.f};

        explicit Spark(glm::vec2 pos);
        void update();
    };

    void push(glm::vec2 position);
    void draw();
    void activate();
    void deactivate();

    std::list<Spark> sparks;

private:
    static constexpr std::size_t spark_life{50};
    static constexpr std::size_t spark_amount{6};
    static constexpr float spark_gravity{2.5f};
    Rect rect_drawable{};
};

#endif //LINHOP_SPARKS_HPP
