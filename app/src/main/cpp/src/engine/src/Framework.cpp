#include "Framework.h"
#include <memory.h>
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <exception>
#include <memory>
#include <functional>
#include "stb_image.h"

#if !defined(__ANDROID__) && !defined(ANDROID)
    #include "GL/glew.h"
    #include "GLFW/glfw3.h"
#endif

#if defined (WIN32) || defined (_WIN32)
    #include <Windows.h>
#endif

void engine_log_message(const char* fmt, ...) {

    size_t size;
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

void engine_check_error() {

    GLenum error = glGetError();

    const char* const prefix = "GL error:";

    while(error != GL_NO_ERROR)
    {
        switch (error) {
            case GL_NO_ERROR:
                LOGE("%s GL_NO_ERROR", prefix); break;
            case GL_INVALID_ENUM:
                LOGE("%s GL_INVALID_ENUM", prefix); break;
            case GL_INVALID_VALUE:
                LOGE("%s GL_INVALID_VALUE", prefix); break;
            case GL_INVALID_OPERATION:
                LOGE("%s GL_INVALID_OPERATION", prefix); break;
            case GL_OUT_OF_MEMORY:
                LOGE("%s GL_OUT_OF_MEMORY", prefix); break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                LOGE("%s GL_INVALID_FRAMEBUFFER_OPERATION", prefix); break;
            default:
                LOGE("GL error: %d", error);
        }

        error = glGetError();
    }
}

unsigned int create_shader(unsigned int shader_type, const char* src) {

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

unsigned int texture_load(const char* path) {

#ifdef ENGINE_FLIP_VERTICALLY
    stbi_set_flip_vertically_on_load(1);
#endif

    int width, height, bpp;
#if defined(__ANDROID__) || defined(ANDROID)

    struct file texture_file;
    file_load_asset(&texture_file, path);
    stbi_uc* buffer = stbi_load_from_memory(
        static_cast<const stbi_uc*>(texture_file.data),
        (int)texture_file.size, &width, &height, &bpp, 4);

    file_unload(&texture_file);
#else
    stbi_uc* buffer = stbi_load(path, &width, &height, &bpp, 4);
#endif

    if (buffer == nullptr) {
        throw std::exception("Failed to load texture");
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (buffer)
        stbi_image_free(buffer);

    return texture;
}

void texture_unload(unsigned int id) {
    glDeleteTextures(1, &id);
}

int file_load(struct file *file, const char *path) {

    if (!path)
        return 0;

    FILE* f = fopen(path, "r"); if(!f) {
        LOGW("IO: Opening file \"%s\" for read failed", path);
        return 0;
    }

    fseek(f, 0, SEEK_END);
    file->size = ftell(f);
    fseek(f, 0, SEEK_SET);

#if defined(ANDROID)

    const char absolute_wd_path[] = "data/data/com.pachuch.linhop/files/";
    size_t parameter_path_length = strlen(path);
    file->path_size = parameter_path_length + sizeof(absolute_wd_path);
    file->path = (char*)malloc(file->path_size);

    memcpy(file->path, absolute_wd_path, sizeof(absolute_wd_path));
    memcpy(file->path + sizeof(absolute_wd_path) - sizeof(char), path, parameter_path_length + 1);

#else

    file->path_size = strlen(path) + sizeof(path[0]);
    if (!(file->path = (char*)malloc(file->path_size * sizeof(char)))) {
        LOGE("Unable to allocate memory");
        return 0;
    }

    memcpy(file->path, path, file->path_size * sizeof(char));

#endif

    if ((file->data = malloc(file->size)) == NULL) {
        LOGE("Unable to allocate memory");
		fclose(f);
        return 0;
    }

    fread(file->data, file->size, 1, f);
    fclose(f);

#ifndef NDEBUG
    LOGI("IO: File read \"%s\"", path);
#endif

    return 1;
}

int file_save(const char* path, void* data, size_t size) {

    if (!path)
        return 0;

    char* new_path = NULL;
    size_t new_length = 0;

#if defined(ANDROID)

    const char absolute_wd_path[] = "data/data/com.pachuch.linhop/files/";

    size_t parameter_path_length = strlen(path);
    new_length = parameter_path_length + sizeof(absolute_wd_path);
    new_path = (char*)malloc(new_length);

    memcpy(new_path, absolute_wd_path, sizeof(absolute_wd_path));
    memcpy(new_path + sizeof(absolute_wd_path) - sizeof(char), path, parameter_path_length + 1);

#else

    new_length = strlen(path) + sizeof(path[0]);
    if ((new_path = (char*)malloc(new_length * sizeof(char))) == NULL) {
		LOGE("Unable to allocate memory");
        return 0;
    }

    memcpy(new_path, path, new_length * sizeof(char));

#endif

    FILE* f = fopen(new_path, "wb"); if(!f) {
        LOGW("IO: Opening file \"%s\" for write failed", new_path);
        return 0;
    }

#ifndef NDEBUG
    LOGI("IO: Writing to file \"%s\"", path);
#endif

    fwrite(data, size, 1, f);
    fclose(f);
    free(new_path);

    return 1;
}

int file_remove(const char* path) {
    return remove(path) == 0;
}

void file_unload(struct file* file) {
    free(file->data);
    free((void*)file->path);
    memset((void*)file, 0, sizeof(struct file));
}

#if defined(__ANDROID__) || defined(ANDROID)
#include <unistd.h>

int file_load_asset(struct file* file, const char* path) {

    if (!engine_instance->asset_mgr)
        return 0;

    AAsset* asset = AAssetManager_open(engine_instance->asset_mgr, path, AASSET_MODE_BUFFER);

    if (asset != NULL) {

        // copy file path
        file->path_size = strnlen(path, 255);
        file->path = (char*)malloc((file->path_size + 1) * sizeof(char));
        memcpy((void*)file->path, path, (file->path_size + 1) * sizeof(char));

        file->size = AAsset_getLength(asset);
        file->data = malloc(file->size);
        memcpy(file->data, AAsset_getBuffer(asset), file->size);
        AAsset_close(asset);

        return 1;

    } else {
        LOGE("ERROR: Failed to read file to string, asset not found. (file: %s)", path);
        return 0;
    }
}

int engine_get_cwd(char* buf, size_t max_size) {
    return getcwd(buf, max_size) != NULL;
}

#elif defined (__unix__) || defined (__unix)

#include "unistd.h"

int file_load_asset(struct file* file, const char* path) {
    return file_load(file, path);
}

int engine_get_cwd(char* buf, size_t max_size) {
    return getcwd(buf, max_size) != NULL;
}

#elif defined (WIN32) || defined (_WIN32)

#include <Windows.h>

int file_load_asset(struct file* file, const char* path) {
    return file_load(file, path);
}

int engine_get_cwd(char* buf, size_t max_size) {
    return GetCurrentDirectoryA(static_cast<DWORD>(max_size), buf) != 0;
}

#endif