#ifndef ENGINE_SHADER
#define ENGINE_SHADER

#include "Framework.h"
#include <vector>
#include <string>
#include <memory>
#include <optional>

namespace wuh {

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

    private:
		enum class Type { VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2 };
        unsigned shader_id[3]{ (unsigned)-1, (unsigned)-1, (unsigned)-1 };
    };

    unsigned id() const { return this->program_id; }

    static unsigned compile(unsigned shader_type, const char* src);
    static unsigned uniform_location(unsigned shader_id, const std::string& name);
    static void uniform_vec2(unsigned id, const Vec2& value);
    static void uniform_vec3(unsigned id, const Vec3& value);
    static void uniform_vec4(unsigned id, const Vec4& value);
    static void uniform_mat4(unsigned id, const Mat4& value);

    void use() const;
    void res(const Vec2& value) const;
    void color(const Vec4& value) const;

    unsigned u_res;
    unsigned u_color;

protected:
    unsigned program_id;
    std::optional<unsigned> vertex_shader_id;
    std::optional<unsigned> fragment_shader_id;
    std::optional<unsigned> geometry_shader_id;
};

} // end of namespace wuh

#endif // ENGINE_SHADER