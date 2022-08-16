#include "object.h"
#include "memory.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

extern struct spige* spige_instance;

// line
void line_load(struct line* line) {

    memset(line, 0, sizeof(struct line));

    line->width = 1.f;
    memcpy(line->color, (vec4){1.f, 1.f, 1.f, 1.f}, sizeof(vec4));

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

    line->shader = create_program(vertex_src, fragment_src);
    line->vbc = 2;

    glGenVertexArrays(1, &line->vao);
    glGenBuffers(sizeof(line->vbo) / sizeof(line->vbo[0]), line->vbo);
    glBindVertexArray(line->vao);

    glBindBuffer(GL_ARRAY_BUFFER, line->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    line->state = STATE_READY;
}

void line_draw(const struct line *line, vec4 ab) {

    glUseProgram(line->shader);

    mat4 projection;
    glm_mat4_identity(projection);
    glm_ortho(0.0f, (GLfloat)spige_instance->width, (GLfloat)spige_instance->height, 0.0f, -1.0f, 1.0f, projection);

    set_uniform_mat4(line->shader, "projection", (GLfloat *) &projection[0][0]);
    set_uniform4f(line->shader, "color", line->color);

    glBindBuffer(GL_ARRAY_BUFFER, line->vbo[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(float), ab);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(line->vao);
    glLineWidth(line->width);
    glDrawArrays(GL_LINES, 0, line->vbc);
}

void line_unload(struct line* line) {

    if (line->state == STATE_OFF)
        return;

    if (line->vao)
        glDeleteVertexArrays(1, &line->vao);
    if (line->vbo[0])
        glDeleteBuffers(1, &line->vbo[0]);
    if (line->ebo)
        glDeleteBuffers(1, &line->ebo);
    if (line->shader)
        glDeleteProgram(line->shader);
}

// triangle
void tri_load(struct tri* tri) {

    memset(tri, 0, sizeof(struct tri));

    struct tri_vertex {
        GLfloat pos[2];
        GLubyte rgba[4];
    } static tri_vertices[3] = {
        {{0.0f, 0.5f},  {0x00, 0xFF, 0x00, 0xFF}},
        {{-0.5f, -0.5f},{0x00, 0x00, 0xFF, 0xFF}},
        {{0.5f, -0.5f}, {0xFF, 0x00, 0x00, 0xFF}},
    };

    tri->vbc = sizeof(tri_vertices) / sizeof(tri_vertices[0]);

    const char* vertex_src =
        "#version 300 es\n"
        "layout(location = 0) in vec2 pos;\n"
        "layout(location = 1) in vec4 color;\n"
        "out vec4 vColor;\n"
        "uniform mat4 model;\n"
        "uniform mat4 projection;\n"
        "void main(){\n"
//        "   gl_Position = vec4(pos, 0.0, 1.0);\n"
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

    tri->shader = create_program(vertex_src, fragment_src);

    glGenVertexArrays(1, &tri->vao);
    glGenBuffers(sizeof(tri->vbo) / sizeof(tri->vbo[0]), tri->vbo);
    glBindVertexArray(tri->vao);

    glBindBuffer(GL_ARRAY_BUFFER, tri->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tri_vertices), tri_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct tri_vertex), (void*) offsetof(struct tri_vertex, pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct tri_vertex), (void*) offsetof(struct tri_vertex, rgba));

    // note that this is allowed, the call to glVertexAttribPointer registered p_scene->vbo_main
    // as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    tri->state = STATE_READY;
}

void tri_draw(const struct tri *tri, vec2 pos) {

    glUseProgram(tri->shader);

    mat4 projection;
    glm_mat4_identity(projection);
    glm_ortho(0.0f, (GLfloat)spige_instance->width, (GLfloat)spige_instance->height, 0.0f, -1.0f, 1.0f, projection);

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (vec3){ pos[0], pos[1], 0.0f });
    glm_scale(model, (vec3){tri->scale[0], tri->scale[1], 1.0f });

    set_uniform_mat4(tri->shader, "projection", (GLfloat *) &projection[0][0]);
    set_uniform_mat4(tri->shader, "model", (GLfloat *) &model[0][0]);

    glBindVertexArray(tri->vao);
    glDrawArrays(GL_TRIANGLES, 0, tri->vbc);
}

void tri_unload(struct tri* tri) {

    if (tri->state == STATE_OFF)
        return;

    if (tri->vao)
        glDeleteVertexArrays(1, &tri->vao);
    if (tri->vbo[0])
        glDeleteBuffers(1, &tri->vbo[0]);
    if (tri->ebo)
        glDeleteBuffers(1, &tri->ebo);
    if (tri->shader)
        glDeleteProgram(tri->shader);
}

// rectangle
void rect_load(struct rect* rect) {

    memset(rect, 0, sizeof(struct rect));

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

    rect->vbc = sizeof(rect_vertices) / sizeof(rect_vertices[0]);

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

    rect->shader = create_program(vertex_src, fragment_src);

    glGenVertexArrays(1, &rect->vao);
    glGenBuffers(sizeof(rect->vbo) / sizeof(rect->vbo[0]), rect->vbo);
    glBindVertexArray(rect->vao);

    glBindBuffer(GL_ARRAY_BUFFER, rect->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertices), rect_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct rect_vertex), (void*) offsetof(struct rect_vertex, pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct rect_vertex), (void*) offsetof(struct rect_vertex, tex));

    // note that this is allowed, the call to glVertexAttribPointer registered p_scene->vbo_main
    // as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    rect->state = STATE_READY;
}

void rect_draw(const struct rect *rect, vec2 pos) {

    glUseProgram(rect->shader);

    mat4 projection;
    glm_mat4_identity(projection);
    glm_ortho(0.0f, (GLfloat)spige_instance->width, (GLfloat)spige_instance->height, 0.0f, -1.0f, 1.0f, projection);

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (vec3){ pos[0], pos[1], 0.0f });
    glm_scale(model, (vec3){rect->scale[0], rect->scale[1], 1.0f });

    set_uniform_mat4(rect->shader, "projection", (GLfloat *) &projection[0][0]);
    set_uniform_mat4(rect->shader, "model", (GLfloat *) &model[0][0]);
    set_uniform4f(rect->shader, "color", (GLfloat *) &rect->color);

    glBindVertexArray(rect->vao);
    glBindTexture(GL_TEXTURE_2D, rect->texture);
    glDrawArrays(GL_TRIANGLES, 0, rect->vbc);
}

void rect_unload(struct rect* rect) {

    if (rect->state == STATE_OFF)
        return;

    if (rect->vao)
        glDeleteVertexArrays(1, &rect->vao);
    if (rect->vbo[0])
        glDeleteBuffers(1, &rect->vbo[0]);
    if (rect->ebo)
        glDeleteBuffers(1, &rect->ebo);
    if (rect->shader)
        glDeleteProgram(rect->shader);
    if (rect->texture)
        texture_unload(rect->texture);
}

void rect_use_texture(struct rect* rect, unsigned int texture) { rect->texture = texture; }

// text
#include <ft2build.h>
#include FT_FREETYPE_H

void text_load(struct text* text, const char* font, float size) {

    memset(text, 0, sizeof(struct text));

    text->scale = 1.f;
    text->size = size;
    memcpy(text->color, (vec4){1.f, 1.f, 1.f, 1.f}, sizeof(vec4));

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

    text->shader = create_program(vertex_src, fragment_src);
    text->vbc = 6; // vertices count

    glGenVertexArrays(1, &text->vao);
    glGenBuffers(sizeof(text->vbo) / sizeof(text->vbo[0]), text->vbo);
    glBindVertexArray(text->vao);

    glBindBuffer(GL_ARRAY_BUFFER, text->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // then initialize and load the FreeType library
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) // all functions return a value different than 0 whenever an error occurred
        LOGE("ERROR::FREETYPE: Could not init FreeType Library");

    // load font as face
    FT_Face face;

#if defined(__ANDROID__) || defined(ANDROID)
    struct file file;
    if (!file_load_asset(&file, font)) {
        LOGE("Cannot open file %s", font);
        FT_Done_FreeType(ft); return;
    }

    if (FT_New_Memory_Face(ft, file.data, (FT_Long)file.size, 0, &face))
        LOGE("ERROR::FREETYPE: Failed to load font");

    file_unload(&file);
#else
    if (FT_New_Face(ft, font, 0, &face))
        LOGE("ERROR::FREETYPE: Failed to load font");
#endif

    // set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, (FT_UInt)size);
    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    text->characters = (struct character*)malloc(CHARACTERS_CAP * sizeof(struct character));

    if (!text->characters)
        LOGE("Unable to allocate memory\n");

    // then for the first 128 ASCII characters, pre-load/compile their characters and store them
    for (GLubyte c = 0; c < CHARACTERS_CAP; c++) // lol see what I did there
    {
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            LOGE("ERROR::FREETYTPE: Failed to load Glyph");
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
        text->characters[c] = (struct character) {
            .texture = texture,
            .size = { (int)face->glyph->bitmap.width, (int)face->glyph->bitmap.rows },
            .bearing = { face->glyph->bitmap_left, face->glyph->bitmap_top },
            .advance = face->glyph->advance.x
        };

//        text->width += (float)face->glyph->bitmap.width;

//        if ((float)face->glyph->bitmap.rows > text->height)
//            text->height = (float)face->glyph->bitmap.rows;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    text->state = STATE_READY;
}

float text_draw(const struct text *text, const char* str, const vec2 pos) {

    float shift = pos[0];

    glActiveTexture(GL_TEXTURE0);
    glUseProgram(text->shader);
    glBindVertexArray(text->vao);

    mat4 projection;
    glm_mat4_identity(projection);
    glm_ortho(0.0f, (GLfloat)spige_instance->width, (GLfloat)spige_instance->height, 0.0f, -1.0f, 1.0f, projection);

    set_uniform_mat4(text->shader, "projection", (GLfloat *) &projection[0][0]);
    set_uniform4f(text->shader, "color", text->color);

    // iterate through all characters
    for (const char* c = str; *c != '\0'; c++) {

        struct character ch = text->characters[*c];

        float xpos = shift + (float)ch.bearing[0] * text->scale;
        float ypos = pos[1] + (float)(text->characters['H'].bearing[1] - ch.bearing[1]) * text->scale;
//        float ypos = pos[1] + (float)(ch.size[1] - ch.bearing[1]) * text->scale;

        float w = (float)ch.size[0] * text->scale;
        float h = (float)ch.size[1] * text->scale;

        // update VBO for each character

//        float vertices[6][4] = {
//            { xpos,     ypos + h,   0.0f, 0.0f },
//            { xpos + w, ypos,       1.0f, 1.0f },
//            { xpos,     ypos,       0.0f, 1.0f },
//
//            { xpos,     ypos + h,   0.0f, 0.0f },
//            { xpos + w, ypos + h,   1.0f, 0.0f },
//            { xpos + w, ypos,       1.0f, 1.0f },
//        };

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            { xpos,     ypos,       0.0f, 0.0f },

            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.texture);
        glBindBuffer(GL_ARRAY_BUFFER, text->vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // now advance cursors for next glyph
        shift += (float)(ch.advance >> 6) * text->scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // return next free x position
    return shift;
}

void text_unload(struct text* text) {

    if (text->state == STATE_OFF)
        return;

    free(text->characters);
}
