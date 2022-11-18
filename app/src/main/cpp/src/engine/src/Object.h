#ifndef ENGINE_OBJECT_H
#define ENGINE_OBJECT_H

#include "Framework.h"
#include "Graphics.h"
#include "Shader.h"
#include <memory>
#define CHARACTERS_CAP 128

struct Drawable {
    Drawable(std::unique_ptr<Shader> shader);
    virtual ~Drawable() {}
    virtual void update() const = 0;
    enum state state;
    unsigned vao;
    unsigned ebo;
    int vbc;

    std::unique_ptr<Shader> shader;
};

struct Line : public Drawable {

    Line();
    Line(std::unique_ptr<Shader> shader);
    ~Line();

    void draw(const Graphics& g, float ab[4], Color c, float width = 1.f) const;
    inline virtual void update() const override {
        Shader::uniform_vec2(resolution.first, resolution.second);
        Shader::uniform_vec4(color.first, color.second);
    };

    unsigned vbo[1];

private:
    std::pair<unsigned, Vec2> resolution{(unsigned)-1, {}};
    std::pair<unsigned, Vec4> color{(unsigned)-1, {}};
};

struct Tri : public Drawable {

    Tri();
    ~Tri();

    void draw(const Graphics& g, float pos[2], Color c) const;
    inline virtual void update() const override {}

    float rot;
    Vec2 scale;
    unsigned vbo[1];
    unsigned loc_projection;
    unsigned loc_model;
};

struct Rect : public Drawable {

    Rect();
    ~Rect();

    void draw(const Graphics& g, float pos[2], Color c) const;
    inline virtual void update() const override {}
    void useTexture(unsigned int texture);

    float rot;
    Vec2 scale;
    unsigned texture;
    unsigned vbo[1];
    unsigned loc_projection;
    unsigned loc_model;
    unsigned loc_color;
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
    inline virtual void update() const override {}

    float scale;
    float size;
    float width;
    float height;
    unsigned vbo[1];
    unsigned loc_projection;
    unsigned loc_color;
    std::unique_ptr<Character> characters;
};

#endif //ENGINE_OBJECT_H
