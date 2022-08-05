#ifndef SPIGE_OBJECT_H
#define SPIGE_OBJECT_H

#include "framework.h"

struct line {
    enum state state;
    GLuint shader;
    GLuint vbo[1];
    GLuint vao;
    GLuint ebo;
    GLsizei vbc;
    GLfloat width;
    vec4 color;
};

struct tri {
    enum state state;
    GLuint shader;
    GLuint vbo[1];
    GLuint vao;
    GLuint ebo;
    GLsizei vbc;
    GLfloat rot;
    vec2 scale;
    vec4 color;
};

struct rect {
    enum state state;
    GLuint shader;
    GLuint texture;
    GLuint vbo[1];
    GLuint vao;
    GLuint ebo;
    GLsizei vbc;
    GLfloat rot;
    vec2 scale;
    vec4 color;
};

struct character {
    GLuint texture; // ID handle of the glyph texture
    GLuint advance; // horizontal offset to advance to next glyph
    ivec2 size;     // size of glyph
    ivec2 bearing;  // offset from baseline to left/top of glyph
};

#define CHARACTERS_CAP 128

struct text {
    enum state state;
    GLuint shader;
    GLuint vbo[1];
    GLuint vao;
    GLuint ebo;
    GLsizei vbc;
    GLfloat scale;
    GLfloat size;
    GLfloat width;
    vec4 color;
    struct character* characters;
};

#ifdef __cplusplus
extern "C" {
#endif

void line_load(struct line* line);
void line_draw(const struct line *line, vec4 ab);
void line_unload(struct line* line);

void tri_load(struct tri* tri);
void tri_draw(const struct tri *tri, vec2 pos);
void tri_unload(struct tri* tri);

void rect_load(struct rect* rect);
void rect_draw(const struct rect *rect, vec2 pos);
void rect_unload(struct rect* rect);
void rect_use_texture(struct rect* rect, GLuint texture);

void text_load(struct text* text, const char* font, float size);
float text_draw(const struct text *text, const char* str, const vec2 pos);
void text_unload(struct text* text);

#ifdef __cplusplus
}
#endif

#endif //SPIGE_OBJECT_H
