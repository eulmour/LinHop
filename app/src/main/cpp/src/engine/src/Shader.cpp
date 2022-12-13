#include "Shader.h"
#include <exception>
#include <array>
#include <fstream>
#include <sstream>
#include <exception>
#include "Framework.h"

#if !defined(__ANDROID__) && !defined(ANDROID)
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#endif

#define SHADER_FN_ORTHO "mat4 ortho(float left, float right, float bottom, float top, float zNear, float zFar) {\n"\
    "return mat4(\n"\
        "vec4(2.0 / (right - left), 0, 0, 0),\n"\
        "vec4(0, 2.0 / (top - bottom), 0, 0),\n"\
        "vec4(0, 0, -2.0 / (zFar - zNear), 0),\n"\
        "vec4(-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zFar + zNear) / (zFar - zNear), 1.0)\n"\
    ");\n"\
"}\n"

Shader::Shader(
    unsigned vertex_id,
    unsigned fragment_id,
    unsigned geometry_id) {

    this->program_id = glCreateProgram();

    if (vertex_id != (unsigned)-1) {
        glAttachShader(this->program_id, vertex_id);
        this->vertex_shader_id = std::make_optional<unsigned>(vertex_id);
    }

    if (fragment_id != (unsigned)-1) {
        glAttachShader(this->program_id, fragment_id);
        this->fragment_shader_id = std::make_optional<unsigned>(fragment_id);
    }

    if (geometry_id != (unsigned)-1) {
        glAttachShader(this->program_id, geometry_id);
        this->geometry_shader_id = std::make_optional<unsigned>(geometry_id);
    }

    glLinkProgram(this->program_id);

    int success;

    // check for linking errors
    glGetProgramiv(this->program_id, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(this->program_id, sizeof(infoLog), NULL, infoLog);
        fprintf(stderr, "Could not link program: %s\n", infoLog);
    }

    if (this->vertex_shader_id.has_value()) {
        glDeleteShader(this->vertex_shader_id.value());
    }

    if (this->fragment_shader_id.has_value()) {
        glDeleteShader(this->fragment_shader_id.value());
    }

    if (this->geometry_shader_id.has_value()) {
        glDeleteShader(this->geometry_shader_id.value());
    }

    this->u_res = Shader::uniform_location(this->id(), "u_res");
    this->u_color = Shader::uniform_location(this->id(), "u_color");
}

Shader::Shader(Shader&& other)
    : program_id(other.id())
    , vertex_shader_id(std::move(other.vertex_shader_id))
    , fragment_shader_id(std::move(other.fragment_shader_id))
    , geometry_shader_id(std::move(other.geometry_shader_id))
    , u_res(std::move(other.u_res))
    , u_color(std::move(other.u_color))
{
    other.program_id = (unsigned)-1;
}

Shader::~Shader() {
    if (this->program_id != (unsigned)-1) {
        glDeleteProgram(this->program_id);
    }
}

unsigned Shader::compile(unsigned shader_type, const char* src) {

    GLuint shader = glCreateShader(shader_type);
    engine_check_error();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
        fprintf(stderr,
        "Could not compile %s shader: %s\n",
        shader_type == GL_VERTEX_SHADER
                ? "vertex"
                : (shader_type == GL_FRAGMENT_SHADER
                    ? "fragment"
                    : "UNKNOWN"
                ),
        infoLog);
        glDeleteShader(shader);
        return (unsigned)-1;
    }

    return shader;
}

void Shader::uniform_vec2(unsigned id, const Vec2& value) { glUniform2f(id, value[0], value[1]); }
void Shader::uniform_vec3(unsigned id, const Vec3& value) { glUniform3f(id, value[0], value[1], value[2]); }
void Shader::uniform_vec4(unsigned id, const Vec4& value) { glUniform4f(id, value[0], value[1], value[2], value[3]); }
void Shader::uniform_mat4(unsigned id, const Mat4& value) { glUniformMatrix4fv(id, 1, GL_FALSE, (GLfloat*) &value[0][0]); }
unsigned Shader::uniform_location(unsigned shader_id, const std::string& name) { return glGetUniformLocation(shader_id, name.c_str()); }
void Shader::use() const { glUseProgram(this->id()); }
void Shader::res(const Vec2& value) const { Shader::uniform_vec2(this->u_res, value); }
void Shader::color(const Vec4& value) const { Shader::uniform_vec4(this->u_color, value); }

Shader Shader::Builder::from_file(const std::string& path) {

    std::fstream stream(path); if (!stream) {
        throw std::runtime_error("Cannot open file");
    }

    Builder::Type type;

    std::array<std::stringstream, 3> ss;
    std::string line;
    while (std::getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = Type::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = Type::FRAGMENT;
            }
            else if (line.find("geometry") != std::string::npos) {
                type = Type::GEOMETRY;
            }
        }
        else {
            ss[static_cast<int>(type)] << line << '\n';
        }
    }

    bool vertex_available = ss[(int)Type::VERTEX].rdbuf()->in_avail() != 0;
    bool fragment_available = ss[(int)Type::FRAGMENT].rdbuf()->in_avail() != 0;
    bool geometry_available = ss[(int)Type::GEOMETRY].rdbuf()->in_avail() != 0;

    if (geometry_available && vertex_available && fragment_available) {
        return this->vertex(ss[static_cast<unsigned>(Type::VERTEX)].str())
            .fragment(ss[static_cast<unsigned>(Type::FRAGMENT)].str())
            .geometry(ss[static_cast<unsigned>(Type::GEOMETRY)].str())
            .build();
    } else if (fragment_available && vertex_available) {
        return this->vertex(ss[static_cast<unsigned>(Type::VERTEX)].str())
            .fragment(ss[static_cast<unsigned>(Type::FRAGMENT)].str())
            .build();
    } else if (vertex_available) {
        return this->vertex(ss[static_cast<unsigned>(Type::VERTEX)].str())
            .build();
    } else {
        throw std::runtime_error("Cannot parse shader file");
    }
}

Shader::Builder& Shader::Builder::vertex(const std::string& shader_src) {

    const std::string src =
        "#version 300 es\n"
		"#ifdef GL_ES\n"
        "precision mediump float;\n"
		"#endif\n"
        "uniform vec2 u_res;\n"
        SHADER_FN_ORTHO + shader_src;

	this->shader_id[static_cast<int>(Shader::Builder::Type::VERTEX)] = Shader::compile(GL_VERTEX_SHADER, src.c_str());
    return *this;
}

Shader::Builder& Shader::Builder::fragment(const std::string& shader_src) {

    const std::string src =
        "#version 300 es\n"

		"#ifdef GL_ES\n"
        "precision mediump float;\n"
		"#endif\n"

        "uniform vec2 u_res;\n"
        "uniform vec4 u_color;\n"
        "out vec4 out_color;\n"
        + shader_src;

	this->shader_id[static_cast<int>(Shader::Builder::Type::FRAGMENT)] = Shader::compile(GL_FRAGMENT_SHADER, src.c_str());
    return *this;
}

Shader::Builder& Shader::Builder::geometry(const std::string& shader_src) {

    const std::string src =
        "#version 300 es\n"
		"#ifdef GL_ES\n"
        "precision mediump float;\n"
		"#endif\n"
        + shader_src;

	this->shader_id[static_cast<int>(Shader::Builder::Type::GEOMETRY)] = Shader::compile(GL_GEOMETRY_SHADER, src.c_str());
    return *this;
}
