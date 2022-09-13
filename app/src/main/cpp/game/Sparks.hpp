#ifndef LINHOP_SPARKS_HPP
#define LINHOP_SPARKS_HPP

#include "spige.h"
#include <list>

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

class Sparks {

public:
    Sparks() = default;
    ~Sparks();

    struct Spark {
        glm::vec2 pos, vel{0.f, 0.f}, size;
        glm::vec4 color{};
        unsigned int life{50};

        explicit Spark(glm::vec2 pos);
        void update();
    };

    void push(glm::vec2 position);
    void draw();
    void activate();
    void deactivate();

    std::list<Spark> sparks;

private:
    static constexpr std::size_t sparkLife{50};
    static constexpr std::size_t sparkAmount{6};
    static constexpr float sparkGravity{2.5f};
    struct rect rectDrawable{};
};

#endif //LINHOP_SPARKS_HPP
