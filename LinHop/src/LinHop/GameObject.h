#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glm/glm.hpp>
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "IndexBuffer.h"

#include "Shader.h"
#include "Texture.h"

#include <map>

class IObject
{
public:
    virtual ~IObject() {}
    virtual void Draw() const = 0;
    //virtual glm::vec2 Move(float dt) = 0;
private:
    virtual void Init() = 0;
};

class GameObject : public IObject
{
public:
    glm::vec2   pos, size, vel;
    glm::vec3   color;
    float       rot;
    bool        bSolid;
    bool        bDestroyed;
    Shader*     pShader = nullptr;
    Texture*    pSprite = nullptr;

    GameObject();
    ~GameObject();
    virtual void Draw() const override = 0;

protected:
    VertexBuffer*       _vb = nullptr;
    VertexBufferLayout* _layout = nullptr;
    VertexArray*        _va = nullptr;
    IndexBuffer*        _ib = nullptr;

    virtual void Init() override = 0;
};

class RectangleObject : public GameObject
{
public:

    RectangleObject() {}
    RectangleObject(
        Shader&     shader,
        glm::vec2   pos,
        glm::vec2   size,
        Texture&    sprite,
        glm::vec3   color       = glm::vec3(1.0f),
        glm::vec2   velocity    = glm::vec2(0.0f, 0.0f));
    virtual void Draw() const override;

private:
    virtual void Init() override;
};

class LineObject : public GameObject
{
public:
    glm::vec2 a_pos{}, b_pos{};
    LineObject() {}
    LineObject(Shader& shader, glm::vec2 a_pos, glm::vec2 b_pos, glm::vec3 color, Texture* texture = nullptr);
    virtual void Draw() const override {}
    virtual void Draw(glm::vec2 a_pos, glm::vec2 b_pos) const;
private:
    virtual void Init() override;
};

struct Character
{
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // size of glyph
    glm::ivec2   Bearing;   // offset from baseline to left/top of glyph
    unsigned int Advance;   // horizontal offset to advance to next glyph
};

class TextObject : public GameObject
{
public:
    unsigned int vbo = 0, vao = 0;
    std::map<char, Character> characters;
    std::vector<Texture*> textures;

    TextObject() {}
    TextObject(std::string text, std::string font, Shader& shader, glm::vec2 pos, glm::vec3 color, unsigned int size);
    virtual void Draw() const override {}
    virtual void Draw(std::string& text, glm::vec2 pos, float scale);
private:
    virtual void Init() override;
};

#endif
