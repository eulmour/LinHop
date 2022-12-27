#ifndef ENGINE_OBJECT_H
#define ENGINE_OBJECT_H

#include "Framework.h"
#include "Graphics.h"
#include "Shader.h"
#include <memory>
#define CHARACTERS_CAP 128

struct Object {

    Vec2& pos() { return this->pos_; }
    float& width() { return this->width_; }
    float& height() { return this->height_; }

	bool clicked(const Vec2& position) {
		return
			position[0] > pos()[0]
			&& position[0] < pos()[0] + this->width()
			&& position[1] > pos()[1]
			&& position[1] < pos()[1] + this->height();
	}

private:
    float width_{ 0.f };
    float height_{ 0.f };
    Vec2 pos_{};
};

struct Drawable {

    Drawable(Shader shader);
    virtual ~Drawable();
    void use() const { shader.use(); }

protected:
    enum state state;
    unsigned vao;
    unsigned ebo;
    int vbc;

    Shader shader;
};

struct Line : public Drawable {

    Line();
    Line(Shader shader);
    ~Line();

    void draw(const Graphics& g, const float ab[4], Color c, float width = 1.f) const;

    unsigned vbo[1];
};

struct Tri : public Drawable {

    Tri();
    ~Tri();

    void draw(const Graphics& g, float pos[2], Color c) const;

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
