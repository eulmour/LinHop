#ifndef SPIGE_OBJECT_H
#define SPIGE_OBJECT_H

#include "Framework.h"

struct line {
    enum state state;
    unsigned shader;
    unsigned vbo[1];
    unsigned vao;
    unsigned ebo;
    int vbc;
    float width;
    vec4 color;
};

struct tri {
    enum state state;
    unsigned shader;
    unsigned vbo[1];
    unsigned vao;
    unsigned ebo;
    int vbc;
    float rot;
    vec2 scale;
    vec4 color;
};

struct rect {
    enum state state;
    unsigned shader;
    unsigned texture;
    unsigned vbo[1];
    unsigned vao;
    unsigned ebo;
    int vbc;
    float rot;
    vec2 scale;
    vec4 color;
};

struct character {
    unsigned texture; // ID handle of the glyph texture
    unsigned advance; // horizontal offset to advance to next glyph
    ivec2 size;     // size of glyph
    ivec2 bearing;  // offset from baseline to left/top of glyph
};

#define CHARACTERS_CAP 128

struct text {
    enum state state;
    unsigned shader;
    unsigned vbo[1];
    unsigned vao;
    unsigned ebo;
    int vbc;
    float scale;
    float size;
    float width;
    vec4 color;
    struct character* characters;
};

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif //SPIGE_OBJECT_H
