#ifndef ENGINE_OBJECT_H
#define ENGINE_OBJECT_H

#include "Framework.h"
#include "Window.h"
#define CHARACTERS_CAP 128

struct Drawable {
    Drawable() = delete;
    Drawable(Vec2 surface_size);
    enum state state;
    Vec2 surface_size;
    unsigned shader;
    unsigned vao;
    unsigned ebo;
    int vbc;
};

struct Line : public Drawable {

    Line(Vec2 surface_size);
    ~Line();

    void draw(float ab[4], Color c);

    unsigned vbo[1];
    float width;
};

struct Tri : public Drawable {

    Tri(Vec2 surface_size);
    ~Tri();

    void draw(float pos[2], Color c);

    unsigned vbo[1];
    float rot;
    Vec2 scale;
};

struct Rect : public Drawable {

    Rect(Vec2 surface_size);
    ~Rect();

    void draw(float pos[2], Color c);
    void useTexture(unsigned int texture);

    unsigned texture;
    unsigned vbo[1];
    float rot;
    Vec2 scale;
};

struct Text : public Drawable {

    struct character {
        unsigned texture; // ID handle of the glyph texture
        unsigned advance; // horizontal offset to advance to next glyph
        IVec2 size;     // size of glyph
        IVec2 bearing;  // offset from baseline to left/top of glyph
    };

    Text() = delete;
    Text(Vec2 surface_size, const char* font, float size);
    ~Text();

    float draw(const char* str, const float pos[2], Color c);

    unsigned vbo[1];
    float scale;
    float size;
    float width;
    float height;
    struct character* characters;
};

#endif //ENGINE_OBJECT_H
