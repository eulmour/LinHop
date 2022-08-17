#ifndef LINHOP_SPARKS_HPP
#define LINHOP_SPARKS_HPP

#include "spige.h"
#include <list>

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

class Sparks {

public:
    Sparks();
    ~Sparks();

    struct Spark
    {
        glm::vec2 pos, vel, size;
        glm::vec4 color{};
        unsigned int life = 50;

        Spark(glm::vec2 pos);
        void Update();
        void Draw();
    };

    void Push(glm::vec2 position);
    void Draw();
    void activate();
    void deactivate();

    std::list<Spark> aSparks;

private:
    static constexpr std::size_t sparkLife = 50;
    static constexpr std::size_t sparkAmount = 6;
    static constexpr float sparkGravity = 2.5f;

    struct rect rectDrawable{};
};

#endif //LINHOP_SPARKS_HPP
