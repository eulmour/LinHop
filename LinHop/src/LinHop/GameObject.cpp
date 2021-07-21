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

    GLCall(glDrawArrays(GL_TRIANGLES, 0, _vb->GetCount()));
}

void RectangleObject::Init()
{
    _va = new VertexArray();
    _layout = new VertexBufferLayout();

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
    _layout->Push<float>(2);								/* push vertex layout (2 bytes position) */
    _layout->Push<float>(2);								/* push vertex layout (2 bytes texture_coord) */
    _va->AddBuffer(*_vb, *_layout);							/* Set vertex data */

    _va->Unbind();
    _vb->Unbind();
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

void LineObject::Draw() const
{
    _vb->Bind();
    _va->Bind();
    pSprite->Bind();

    GLCall(glDrawArrays(GL_LINES, 0, _vb->GetCount()));
}

void LineObject::Draw(glm::vec2 a_pos, glm::vec2 b_pos) const
{
    float vertices[4] = {
        a_pos.x, a_pos.y,
        b_pos.x, b_pos.y,
    };

    _vb->Bind();
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    _va->Bind();
    pSprite->Bind();

    GLCall(glDrawArrays(GL_LINES, 0, _vb->GetCount()));
}

void LineObject::Init()
{
    _va = new VertexArray();
    _layout = new VertexBufferLayout();

    _vb = new VertexBuffer(nullptr, 4 * sizeof(float), true);	/* Init buffer */
    _layout->Push<float>(2);								    /* push vertex layout (2 bytes position) */
    _va->AddBuffer(*_vb, *_layout);							    /* Set vertex data */

    _va->Unbind();
    _vb->Unbind();
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

void TextObject::Draw() const
{
}

void TextObject::Draw(std::string& text, glm::vec2 pos, unsigned int scale)
{
    //_vb->Bind();
    //_va->Bind();

    // activate corresponding render state	

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->vao);

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
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph
        pos.x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextObject::Init()
{
    // configure VAO/VBO for texture quads
    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
