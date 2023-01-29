#include "Framework.h"
#include "Graphics.h"
#include "File.h"
#include <memory.h>
#include <stdarg.h>
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <stdexcept>
#include <memory>
#include <functional>
#include <sstream>
#include <unordered_map>
#include "stb_image.h"

#if !defined(__ANDROID__) && !defined(ANDROID)
    #include "GL/glew.h"
    #include "GLFW/glfw3.h"
#endif

#if defined (WIN32) || defined (_WIN32)
    #include <Windows.h>
#endif

namespace wuh {

void engine_log_message(const char* fmt, ...) {

    std::size_t size;
    va_list args, tmp_args;
    char* data = 0;

    va_start(args, fmt);
    va_copy(tmp_args, args);

#if defined (WIN32) || defined (_WIN32)
    size = _vscprintf(fmt, tmp_args) + 1;
#else
    size = vsnprintf(NULL, 0, fmt, tmp_args) + 1;
#endif
    va_end(tmp_args);

    if (size > 0) {

        if ((data = (char*)malloc(size)) == NULL) {
            LOGE_PRINT("Unable to allocate memory");
			// MessageBoxA(NULL, data, "Unable to allocate memory.", MB_ICONERROR | MB_OK);
            return;
        }

        if (vsnprintf(data, 10*1024*1024, fmt, args) < 0) {
            data[size - 1] = 0;
        }

#ifdef ENGINE_WRITE_LOGS
		static std::unique_ptr<FILE, std::function<void(FILE*)>> log_file(fopen("log.txt", "w"), [](FILE* f) {
            fclose(f);
		});

		if (!log_file.get()) {
			LOGE_PRINT("Error: opening file \"log.txt\" for write failed");
        } else {
			fputs(data, log_file.get());
        }
#else
    #if defined (WIN32) || defined (_WIN32)
        MessageBoxA(NULL, data, "Message", MB_OK);
    #else
        printf("%s", data);
    #endif
#endif
        free(data);
    }

    va_end(args);
    errno = 0;
}

unsigned int create_shader(unsigned int shader_type, const char* src) {

    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
        LOGE(
        "Could not compile %s shader: %s",
        shader_type == GL_VERTEX_SHADER
                ? "vertex"
                : (shader_type == GL_FRAGMENT_SHADER
                    ? "fragment"
                    : "UNKNOWN"
                ),
        infoLog);
        glDeleteShader(shader);
        abort();
    }

    return shader;
}

unsigned int create_program(const char* vertex_src, const char* fragment_src) {

    // link shaders
    GLuint program = glCreateProgram();

    GLuint vertexShader = create_shader(GL_VERTEX_SHADER, vertex_src);
    GLuint fragmentShader = create_shader(GL_FRAGMENT_SHADER, fragment_src);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    char infoLog[512];

    // check for linking errors
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog);
        LOGE("Could not link program: %s", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

unsigned int texture_create(int width, int height, const void* data) {

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

unsigned int texture_load_from_file(const char* path) {

#ifdef ENGINE_FLIP_VERTICALLY
    stbi_set_flip_vertically_on_load(1);
#endif

    int width, height, bpp;
#if defined(__ANDROID__) || defined(ANDROID)

    File texture_file = File::asset(path);

    stbi_uc* buffer = stbi_load_from_memory(
        static_cast<const stbi_uc*>(texture_file.data()),
        (int)texture_file.size(), &width, &height, &bpp, 4);
#else
    if (!File::exists(path)) {
        throw std::runtime_error("File " + std::string(path) + " does not exist");
    }
    stbi_uc* buffer = stbi_load(path, &width, &height, &bpp, 4);
#endif

    if (buffer == nullptr) {
        throw std::runtime_error("Failed to load " + std::string(path) + ", " + std::string(stbi_failure_reason()));
    }

    unsigned texture = texture_create(width, height, buffer);
    stbi_image_free(buffer);

    return texture;
}

void texture_unload(unsigned int id) {
    glDeleteTextures(1, &id);
}

} // end of namespace wuh