#include "Object.h"
#include "Internal.h"
#include <cstring>

#if !defined(__ANDROID__) && !defined(ANDROID)
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#endif

// line
Line::Line() : width(1.f) {

    const char* vertex_src =
        "#version 300 es\n"
        "layout(location = 0) in vec2 pos;\n"
        "uniform mat4 projection;\n"
        "void main(){\n"
        "   gl_Position = projection * vec4(pos, 0.0, 1.0);\n"
        "}\0";

    const char* fragment_src =
        "#version 300 es\n"
        "precision mediump float;\n"
        "out vec4 fragColor;\n"
        "uniform vec4 color;\n"
        "void main() {\n"
        "   fragColor = color;\n"
        "}\0";

    this->shader = create_program(vertex_src, fragment_src);
    this->vbc = 2;

    glGenVertexArrays(1, &this->vao);
    glGenBuffers(sizeof(this->vbo) / sizeof(this->vbo[0]), this->vbo);
    glBindVertexArray(this->vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    this->state = STATE_READY;
}

void Line::draw(const Graphics& g, float ab[4], Color c) const {

    glUseProgram(this->shader);

    glm::mat4 projection = glm::ortho(
        0.0f,
        static_cast<GLfloat>(g.viewport()[0]),
        static_cast<GLfloat>(g.viewport()[1]),
        0.0f,
        -1.0f,
        1.0f
    );

    set_uniform_mat4(this->shader, "projection", (GLfloat *) &projection[0][0]);
    set_uniform4f(this->shader, "color", c);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(float), ab);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(this->vao);
    glLineWidth(this->width);
    glDrawArrays(GL_LINES, 0, this->vbc);
}

Line::~Line() {

    if (this->state == STATE_OFF)
        return;

    if (this->vao)
        glDeleteVertexArrays(1, &this->vao);
    if (this->vbo[0])
        glDeleteBuffers(1, &this->vbo[0]);
    if (this->ebo)
        glDeleteBuffers(1, &this->ebo);
    if (this->shader)
        glDeleteProgram(this->shader);
}

// triangle
Tri::Tri() {

    struct tri_vertex {
        GLfloat pos[2];
        GLubyte rgba[4];
    } static tri_vertices[3] = {
        {{0.0f, 0.5f},  {0x00, 0xFF, 0x00, 0xFF}},
        {{-0.5f, -0.5f},{0x00, 0x00, 0xFF, 0xFF}},
        {{0.5f, -0.5f}, {0xFF, 0x00, 0x00, 0xFF}},
    };

    this->vbc = sizeof(tri_vertices) / sizeof(tri_vertices[0]);

    const char* vertex_src =
        "#version 300 es\n"
        "layout(location = 0) in vec2 pos;\n"
        "layout(location = 1) in vec4 color;\n"
        "out vec4 vColor;\n"
        "uniform mat4 model;\n"
        "uniform mat4 projection;\n"
        "void main(){\n"
        "   gl_Position = projection * model * vec4(pos, 0.0, 1.0);\n"
        "   vColor = color;\n"
        "}\0";

    const char* fragment_src =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec4 vColor;\n"
        "out vec4 fragColor;\n"
        "void main() {\n"
        "   fragColor = vColor;\n"
        "}\0";

    this->shader = create_program(vertex_src, fragment_src);

    glGenVertexArrays(1, &this->vao);
    glGenBuffers(sizeof(this->vbo) / sizeof(this->vbo[0]), this->vbo);
    glBindVertexArray(this->vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tri_vertices), tri_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct tri_vertex), (void*) offsetof(struct tri_vertex, pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct tri_vertex), (void*) offsetof(struct tri_vertex, rgba));

    // note that this is allowed, the call to glVertexAttribPointer registered p_scene->vbo_main
    // as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    this->state = STATE_READY;
}

void Tri::draw(const Graphics& g, float pos[2], Color c) const {

    (void)c;
    glUseProgram(this->shader);

    glm::mat4 projection = glm::ortho(
        0.0f,
        static_cast<GLfloat>(g.viewport()[0]),
        static_cast<GLfloat>(g.viewport()[1]),
        0.0f,
        -1.0f,
        1.0f
    );

    glm::mat4 model(1.0f);

    model = glm::translate(model, glm::vec3(pos[0], pos[1], 0.0f));
    model = glm::rotate(model, glm::radians(this->rot), glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(this->scale[0], this->scale[1], 1.0f));

    set_uniform_mat4(this->shader, "projection", (GLfloat *) &projection[0][0]);
    set_uniform_mat4(this->shader, "model", (GLfloat *) &model[0][0]);

    glBindVertexArray(this->vao);
    glDrawArrays(GL_TRIANGLES, 0, this->vbc);
}

Tri::~Tri() {

    if (this->state == STATE_OFF)
        return;

    if (this->vao)
        glDeleteVertexArrays(1, &this->vao);
    if (this->vbo[0])
        glDeleteBuffers(1, &this->vbo[0]);
    if (this->ebo)
        glDeleteBuffers(1, &this->ebo);
    if (this->shader)
        glDeleteProgram(this->shader);
}

// rectangle
Rect::Rect() {

    struct rect_vertex {
        GLfloat pos[2];
        GLfloat tex[2];
    } static rect_vertices[6] = {
        {{0.f, 1.f}, {0.f, 1.f}},
        {{1.f, 0.f}, {1.f, 0.f}},
        {{0.f, 0.f}, {0.f, 0.f}},

        {{0.f, 1.f}, {0.f, 1.f}},
        {{1.f, 1.f}, {1.f, 1.f}},
        {{1.f, 0.f}, {1.f, 0.f}},
    };

    this->vbc = sizeof(rect_vertices) / sizeof(rect_vertices[0]);

    const char* vertex_src =
        "#version 300 es\n"
        "layout(location = 0) in vec2 pos;\n"
        "layout(location = 1) in vec2 tex;\n"

        "out vec2 tex_coord;\n"

        "uniform mat4 model;\n"
        "uniform mat4 projection;\n"

        "void main(){\n"
        "   gl_Position = projection * model * vec4(pos, 0.0, 1.0);\n"
        "   tex_coord = tex;\n"
        "}\0";

    const char* fragment_src =
        "#version 300 es\n"
        "precision mediump float;\n"

        "layout(location = 0) out vec4 fragColor;\n"

        "in vec2 tex_coord;\n"

        "uniform sampler2D sprite;\n"
        "uniform vec4 color;\n"

        "void main() {\n"
        "   fragColor = color * texture(sprite, tex_coord);\n"
        "}\0";

    this->shader = create_program(vertex_src, fragment_src);

    glGenVertexArrays(1, &this->vao);
    glGenBuffers(sizeof(this->vbo) / sizeof(this->vbo[0]), this->vbo);
    glBindVertexArray(this->vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertices), rect_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct rect_vertex), (void*) offsetof(struct rect_vertex, pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct rect_vertex), (void*) offsetof(struct rect_vertex, tex));

    // note that this is allowed, the call to glVertexAttribPointer registered p_scene->vbo_main
    // as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    this->state = STATE_READY;
}

void Rect::draw(const Graphics& g, float pos[2], Color c) const {

    glUseProgram(this->shader);

    glm::mat4 projection = glm::ortho(
        0.0f,
        static_cast<GLfloat>(g.viewport()[0]),
        static_cast<GLfloat>(g.viewport()[1]),
        0.0f,
        -1.0f,
        1.0f
    );

    glm::mat4 model(1.0f);

    model = glm::translate(model, glm::vec3(pos[0], pos[1], 0.0f));
    // model = glm::rotate(model, glm::radians(this->rot), glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(this->scale[0], this->scale[1], 1.0f));

    set_uniform_mat4(this->shader, "projection", (GLfloat *) &projection[0][0]);
    set_uniform_mat4(this->shader, "model", (GLfloat *) &model[0][0]);
    set_uniform4f(this->shader, "color", c);

    glBindVertexArray(this->vao);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    glDrawArrays(GL_TRIANGLES, 0, this->vbc);
}

Rect::~Rect() {

    if (this->state == STATE_OFF)
        return;

    if (this->vao)
        glDeleteVertexArrays(1, &this->vao);
    if (this->vbo[0])
        glDeleteBuffers(1, &this->vbo[0]);
    if (this->ebo)
        glDeleteBuffers(1, &this->ebo);
    if (this->shader)
        glDeleteProgram(this->shader);
    if (this->texture)
        texture_unload(this->texture);
}

void Rect::useTexture(unsigned int texture) { this->texture = texture; }

// text
#include <ft2build.h>
#include FT_FREETYPE_H

Text::Text(const char* font, float size) {

    this->scale = 1.f;
    this->size = size;

    const char* vertex_src =
        "#version 300 es\n"
        "layout(location = 0) in vec4 vx;\n"
        "out vec2 v_tex_coord;\n"
        "uniform mat4 projection;\n"
        "void main(){\n"
        "   gl_Position = projection * vec4(vx.xy, 0.0, 1.0);\n"
        "   v_tex_coord = vx.zw;\n"
        "}\0";

    const char* fragment_src =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec2 v_tex_coord;\n"
        "out vec4 fragColor;\n"
        "uniform sampler2D text;\n"
        "uniform vec4 color;\n"
        "void main() {\n"
        "   vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, v_tex_coord).r);\n"
        "   fragColor = color * sampled;\n"
        "}\0";

    this->shader = create_program(vertex_src, fragment_src);
    this->vbc = 6; // vertices count

    glGenVertexArrays(1, &this->vao);
    glGenBuffers(sizeof(this->vbo) / sizeof(this->vbo[0]), this->vbo);
    glBindVertexArray(this->vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // then initialize and load the FreeType library
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) { // all functions return a value different than 0 whenever an error occurred
        throw std::exception("FreeType: Could not init FreeType Library");
    }

    // load font as face
    FT_Face face;

#if defined(__ANDROID__) || defined(ANDROID)

    struct file file;
    if (!file_load_asset(&file, font)) {
        LOGE("Cannot open file %s\n", font);
        FT_Done_FreeType(ft); return;
    }

    if (FT_New_Memory_Face(ft, static_cast<const FT_Byte*>(file.data), (FT_Long)file.size, 0, &face))
        LOGE("FreeType: Failed to load font\n");

    file_unload(&file);

#else

    if (!engine_file_exists_(font)) {
        throw std::exception("Failed to load font, file does not exists.");
    }

    if (FT_New_Face(ft, font, 0, &face)) {
        throw std::exception("FreeType: Failed to load font.");
    }

#endif

    // set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, (FT_UInt)size);
    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // this->characters = (struct character*)malloc(CHARACTERS_CAP * sizeof(struct character));
    this->characters = std::unique_ptr<Character>(new Character[CHARACTERS_CAP]);

    if (this->characters == nullptr) {
        throw std::exception("Unable to allocate memory");
    }

    // then for the first 128 ASCII characters, pre-load/compile their characters and store them
    for (GLubyte c = 0; c < CHARACTERS_CAP; c++) // lol see what I did there
    {
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            LOGE("FreeType: Failed to load Glyph\n");
            continue;
        }

        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_LUMINANCE,
            (GLsizei)face->glyph->bitmap.width,
            (GLsizei)face->glyph->bitmap.rows,
            0,
            GL_LUMINANCE,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // now store character for later use
        this->characters.get()[c] = Character {
            texture, // texture
            static_cast<unsigned int>(face->glyph->advance.x), // advance
            { (int)face->glyph->bitmap.width, (int)face->glyph->bitmap.rows }, // size
            { face->glyph->bitmap_left, face->glyph->bitmap_top } // bearing
        };

        this->width += (float)(face->glyph->advance.x >> 6) * this->scale; // TODO find text width and height
        float height = (float)(face->glyph->advance.y >> 6) * this->scale;

        if (height > this->height)
            this->height = height;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    this->state = STATE_READY;
}

float Text::draw(const Graphics& g, const char* str, const float pos[2], Color c) const {

    float shift = pos[0];

    glActiveTexture(GL_TEXTURE0);
    glUseProgram(this->shader);
    glBindVertexArray(this->vao);

    glm::mat4 projection = glm::ortho(
        0.0f,
        static_cast<GLfloat>(g.viewport()[0]),
        static_cast<GLfloat>(g.viewport()[1]),
        0.0f,
        -1.0f,
        1.0f
    );

    set_uniform_mat4(this->shader, "projection", (GLfloat *) &projection[0][0]);
    set_uniform4f(this->shader, "color", c);

    // iterate through all characters
    for (const char* c = str; *c != '\0'; c++) {

        // struct character ch = this->characters[*c];
        Character ch = this->characters.get()[*c];

        float xpos = shift + (float)ch.bearing[0] * this->scale;
        float ypos = pos[1] + (float)(this->characters.get()['H'].bearing[1] - ch.bearing[1]) * this->scale;

        float w = (float)ch.size[0] * this->scale;
        float h = (float)ch.size[1] * this->scale;

        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            { xpos,     ypos,       0.0f, 0.0f },

            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.texture);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // now advance cursors for next glyph
        shift += (float)(ch.advance >> 6) * this->scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // return next free x position
    return shift;
}

Text::~Text() = default;
