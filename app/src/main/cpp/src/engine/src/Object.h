#ifndef ENGINE_OBJECT_H
#define ENGINE_OBJECT_H

#include "Framework.h"
#include "Graphics.h"
#include <memory>
#define CHARACTERS_CAP 128

struct Drawable {
    enum state state;
    unsigned shader;
    unsigned vao;
    unsigned ebo;
    int vbc;
};

struct Line : public Drawable {

    Line();
    ~Line();

    void draw(const Graphics& g, float ab[4], Color c) const;

    unsigned vbo[1];
    float width;
};

struct Tri : public Drawable {

    Tri();
    ~Tri();

    void draw(const Graphics& g, float pos[2], Color c) const;

    unsigned vbo[1];
    float rot;
    Vec2 scale;
};

struct Rect : public Drawable {

    Rect();
    ~Rect();

    void draw(const Graphics& g, float pos[2], Color c) const;
    void useTexture(unsigned int texture);

    unsigned texture;
    unsigned vbo[1];
    float rot;
    Vec2 scale;
};

struct Text : public Drawable {

    struct Character {
        unsigned texture; // ID handle of the glyph texture
        unsigned advance; // horizontal offset to advance to next glyph
        IVec2 size;     // size of glyph
        IVec2 bearing;  // offset from baseline to left/top of glyph
    };

    Text() = delete;
    Text(const char* font, float size);
    ~Text();

    float draw(const Graphics& g, const char* str, const float pos[2], Color c) const;

    unsigned vbo[1];
    float scale;
    float size;
    float width;
    float height;
    // struct character* characters;
    std::unique_ptr<Character> characters;
};

#endif //ENGINE_OBJECT_H
