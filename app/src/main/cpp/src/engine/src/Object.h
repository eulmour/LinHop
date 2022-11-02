#ifndef ENGINE_OBJECT_H
#define ENGINE_OBJECT_H

#include "Framework.h"
#define CHARACTERS_CAP 128

struct Drawable {
    enum state state;
    unsigned shader;
    unsigned vao;
    unsigned ebo;
    int vbc;
    float rot;
    Vec2 scale;
    Color color;
};

struct line : public Drawable {
    unsigned vbo[1];
    float width;
};

struct tri : public Drawable {
    unsigned vbo[1];
};

struct rect : public Drawable {
    unsigned vbo[1];
};

struct text : public Drawable {

    struct character {
        unsigned texture; // ID handle of the glyph texture
        unsigned advance; // horizontal offset to advance to next glyph
        IVec2 size;     // size of glyph
        IVec2 bearing;  // offset from baseline to left/top of glyph
    };

    unsigned vbo[1];
    float scale;
    float size;
    float width;
    float height;
    struct character* characters;
};

void line_load(struct line* line);
void line_draw(const struct line *line, float ab[4]);
void line_unload(struct line* line);

void tri_load(struct tri* tri);
void tri_draw(const struct tri *tri, float pos[2]);
void tri_unload(struct tri* tri);

void rect_load(struct rect* rect);
void rect_draw(const struct rect *rect, float pos[2]);
void rect_unload(struct rect* rect);
void rect_use_texture(struct rect* rect, unsigned int texture);

void text_load(struct text* text, const char* font, float size);
float text_draw(const struct text *text, const char* str, const float pos[2]);
void text_unload(struct text* text);

#endif //ENGINE_OBJECT_H
