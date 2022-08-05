#ifndef LINHOP_LAZERS_HPP
#define LINHOP_LAZERS_HPP

#include "spige.h"
#include <list>

#define LAZERS_WIDTH (static_cast<float>(spige_instance->width) / 3.f)

class Lazers {

public:
    Lazers(struct line* linedrawable);
    ~Lazers();

    void Trigger(float position);
    void Draw();
    void activate();
    void deactivate();

    class Lazer
    {
    public:
        vec2 a, b;

        Lazer(vec2 a, vec2 b);
    };

    unsigned int liveTime = 0U;
    float pos;

    static constexpr std::size_t lazersLives = 240;

    std::list<Lazer> lazers;
private:
    struct line* lineDrawable = nullptr;
    struct rect rectDrawable{};
};

#endif //LINHOP_LAZERS_HPP
