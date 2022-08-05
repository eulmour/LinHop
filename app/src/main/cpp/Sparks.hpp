#ifndef LINHOP_SPARKS_HPP
#define LINHOP_SPARKS_HPP

#include "spige.h"
#include <list>

class Sparks {

public:
    Sparks();
    ~Sparks();

    struct Spark
    {
        vec2 pos, vel, size;
        vec4 color{};
        unsigned int life = 50;

        Spark(vec2 pos);
        void Update();
        void Draw();
    };

    void Push(vec2 position);
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
