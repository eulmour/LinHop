#include "GLCall.h"
#include "GameObject.h"

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>

GameObject::GameObject() :
    pos(0.0f, 0.0f),
    size(1.0f, 1.0f),
    vel(0.0f),
    color(1.0f),
    rot(0.0f),
    bSolid(false),
    bDestroyed(false) { }

GameObject::~GameObject()
{
    delete _vb;
    delete _va;
    delete _ib;
    delete _layout;
}

RectangleObject::RectangleObject(
    Shader&     shader,
    glm::vec2   pos,
    glm::vec2   size,
    Texture&    sprite,
    glm::vec3   color,
    glm::vec2   velocity)
{
    this->pos           = pos;
    this->size          = size;
    this->vel           = velocity;
    this->color         = color;
    this->rot           = 0.0f;
    this->pShader       = &shader,
    this->pSprite       = &sprite;
    this->bSolid        = false;
    this->bDestroyed    = false;

    Init();
}

void RectangleObject::Draw() const
{
    _vb->Bind();
    _va->Bind();
    pSprite->Bind();

    GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
}

void RectangleObject::Init()
{
    _va = new VertexArray();
    _va->Bind();

    float vertices[] = {
        /*pos*/     /*tex*/
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    _vb = new VertexBuffer(vertices, sizeof(vertices));		/* Init buffer */
    
    _layout = new VertexBufferLayout();
    _layout->Push<float>(2);								/* push vertex layout (2 bytes position) */
    _layout->Push<float>(2);								/* push vertex layout (2 bytes texture_coord) */
    _va->AddBuffer(*_vb, *_layout);							/* Set vertex data */

    _vb->Unbind();
    _va->Unbind();
}

LineObject::LineObject(Shader& shader, glm::vec2 a_pos, glm::vec2 b_pos, glm::vec3 color, Texture* texture)
{
    this->a_pos = a_pos;
    this->b_pos = b_pos;
    this->color = color;
    this->rot = 0.0f;
    this->pShader = &shader;
    this->pSprite = texture;
    this->bSolid = false;
    this->bDestroyed = false;
    Init();
}

void LineObject::Draw(glm::vec2 a, glm::vec2 b) const
{
    float vertices[4] =
    {
        a.x, a.y,
        b.x, b.y
    };

    pSprite->Bind();
    _vb->Bind();
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    _vb->Unbind();

    _va->Bind();
    GLCall(glDrawArrays(GL_LINES, 0, 2));
    _va->Unbind();

    pSprite->Unbind();
}

void LineObject::Init()
{
    _va = new VertexArray();
    _va->Bind();

    _vb = new VertexBuffer(nullptr, 4 * sizeof(float), true);	/* Init buffer */

    _layout = new VertexBufferLayout();
    _layout->Push<float>(2);								    /* push vertex layout (2 bytes position) */
    _va->AddBuffer(*_vb, *_layout);							    /* Set vertex data */

    _vb->Unbind();
    _va->Unbind();
}

TextObject::TextObject(std::string text, std::string font, Shader& shader, glm::vec2 pos, glm::vec3 color, unsigned int size)
{
    this->pos = pos;
    this->color = color;
    this->pShader = &shader;
    this->pSprite = nullptr;
    this->bSolid = false;
    this->bDestroyed = false;
    
    Init();

    // then initialize and load the FreeType library
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) // all functions return a value different than 0 whenever an error occurred
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, font.c_str(), 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    // set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, size);
    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // then for the first 128 ASCII characters, pre-load/compile their characters and store them
    for (GLubyte c = 0; c < 128; c++) // lol see what I did there 
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        characters.insert(std::pair<char, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void TextObject::Draw(std::string& text, glm::vec2 pos, unsigned int scale)
{
    glActiveTexture(GL_TEXTURE0);
    _va->Bind();

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = characters[*c];

        float xpos = pos.x + ch.Bearing.x * scale;
        float ypos = pos.y + (this->characters['H'].Bearing.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            { xpos,     ypos,       0.0f, 0.0f },

            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f }
        };
        
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        
        _vb->Bind();
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        _vb->Unbind();

        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // now advance cursors for next glyph
        pos.x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
    }

    _va->Unbind();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextObject::Init()
{
    _va = new VertexArray();
    _va->Bind();

    _vb = new VertexBuffer(nullptr, sizeof(float) * 6 * 4, true);

    _layout = new VertexBufferLayout();

    _layout->Push<float>(2); /* vertex */
    _layout->Push<float>(2); /* texture */
    _va->AddBuffer(*_vb, *_layout);

    _vb->Unbind();
    _va->Unbind();
}
