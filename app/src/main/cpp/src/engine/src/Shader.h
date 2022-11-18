#ifndef ENGINE_SHADER
#define ENGINE_SHADER

#include "Framework.h"
#include <vector>
#include <string>
#include <memory>
#include <optional>

#define SHADER_FN_ORTHO "mat4 ortho(float left, float right, float bottom, float top, float zNear, float zFar) {\n"\
    "return mat4(\n"\
        "vec4(2.0 / (right - left), 0, 0, 0),\n"\
        "vec4(0, 2.0 / (top - bottom), 0, 0),\n"\
        "vec4(0, 0, -2.0 / (zFar - zNear), 0),\n"\
        "vec4(-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zFar + zNear) / (zFar - zNear), 1.0)\n"\
    ");\n"\
"}\n"

struct Shader {

    Shader() = delete;
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other);
    Shader(unsigned vertex_id, unsigned fragment_id, unsigned geometry_id);
    ~Shader();

    struct Builder {
        Builder& vertex(const std::string& shader_src);
        Builder& fragment(const std::string& shader_src);
        Builder& geometry(const std::string& shader_src);
        Shader from_file(const std::string& path);

        inline Shader build() {
            return Shader(
                this->shader_id[static_cast<int>(Shader::Builder::Type::VERTEX)],
                this->shader_id[static_cast<int>(Shader::Builder::Type::FRAGMENT)],
                this->shader_id[static_cast<int>(Shader::Builder::Type::GEOMETRY)]
            );
        }

        inline std::unique_ptr<Shader> mk_unique() {
            return std::make_unique<Shader>(
                this->shader_id[static_cast<int>(Shader::Builder::Type::VERTEX)],
                this->shader_id[static_cast<int>(Shader::Builder::Type::FRAGMENT)],
                this->shader_id[static_cast<int>(Shader::Builder::Type::GEOMETRY)]
            );
        }

        void update(Shader& my_shader);

    private:
		enum class Type { VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2 };
        unsigned shader_id[3]{ (unsigned)-1, (unsigned)-1, (unsigned)-1 };
    };

    unsigned id() const { return this->program_id; }

    static unsigned compile(unsigned shader_type, const char* src);
    static void uniform_vec2(unsigned shader_id, const Vec2& value);
    static void uniform_vec3(unsigned shader_id, const Vec3& value);
    static void uniform_vec4(unsigned shader_id, const Vec4& value);
    static void uniform_mat4(unsigned shader_id, const Mat4& value);
    static unsigned uniform_location(unsigned shader_id, const std::string& name);

protected:
    unsigned program_id;

    std::optional<unsigned> vertex_shader_id;
    std::optional<unsigned> fragment_shader_id;
    std::optional<unsigned> geometry_shader_id;
};

#endif // ENGINE_SHADER