#include "framework.h"
#include <assert.h>
#include <memory.h>
#include <string.h>
#include "stb_image.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

// global
struct spige* spige_instance;

bool spige_init(struct spige* app) {

    memset(app, 0, sizeof(struct spige));

//    // Check openGL on the system
    int opengl_info[] = { GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS };

    for (int value = 0; value != sizeof(opengl_info) / sizeof(int); value++) {
        LOGI("OpenGL Info: %s", glGetString(opengl_info[value]));
    }

    // Initialize GL state.
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    spige_instance = app;
    return true;
}

void spige_viewport(struct spige* app, int w, int h) {

    app->width = w;
    app->height = h;
    glViewport(0, 0, w, h);
}

void spige_destroy(struct spige* app) {

}

void spige_check_error() {

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
    spige_check_error();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
        LOGE(
        "Could not compile %s shader: %s\n",
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
        LOGE("Could not link program: %s\n", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

unsigned int texture_load(const char* path) {

#ifdef SPIGE_FLIP_VERTICALLY
    stbi_set_flip_vertically_on_load(1);
#endif

    int width, height, bpp;
#if defined(__ANDROID__) || defined(ANDROID)

    struct file texture_file;
    file_load_asset(&texture_file, path);
    stbi_uc* buffer = stbi_load_from_memory(
        texture_file.data, (int)texture_file.size, &width, &height, &bpp, 4);

    file_unload(&texture_file);
#else
    stbi_uc* buffer = stbi_load(path, &width, &height, &bpp, 4);
#endif

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

void set_uniform_mat4(unsigned int program, const char* name, float* value) {

    GLint location = glGetUniformLocation(program, name);

    if (location == -1) {
        LOGW("Uniform %s does not exists\n", name);
    }

    glUniformMatrix4fv(location, 1, GL_FALSE, value);
}

void set_uniform4f(unsigned int program, const char* name, const vec4 value) {

    GLint location = glGetUniformLocation(program, name);

    if (location == -1) {
        LOGW("Uniform %s does not exists\n", name);
    }

    glUniform4f(location, value[0], value[1], value[2], value[3]);
}

int file_load(struct file *file, const char *path) {

    if (!path)
        return 0;

#if defined(ANDROID)

    size_t parameter_path_length = strlen(path);
    file->path_size = parameter_path_length + sizeof(ABSOLUTE_WD_PATH);
    file->path = (char*)malloc(file->path_size);

    memcpy(file->path, ABSOLUTE_WD_PATH, sizeof(ABSOLUTE_WD_PATH));
    memcpy(file->path + sizeof(ABSOLUTE_WD_PATH) - sizeof(char), path, parameter_path_length + 1);

#else

    file->path_size = strlen(path) + sizeof(path[0]);
    if (!(file->path = (char*)malloc(file->path_size * sizeof(char)))) {
        LOGE("Unable to allocate memory.\n");
        return 0;
    }

    memcpy(file->path, path, file->path_size * sizeof(char));

#endif

    FILE* f = fopen(file->path, "r"); if(!f) {
        LOGE("Error: opening file \"%s\" for read failed.\n", file->path);
        free(file->path);
        return 0;
    }

#ifdef DEBUG
    LOGI("reading file \"%s\".\n", file->path);
#endif

    fseek(f, 0, SEEK_END);
    file->size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if ((file->data = malloc(file->size)) == NULL) {
        LOGE("Unable to allocate memory.\n");
		fclose(f);
        return 0;
    }

    fread(file->data, file->size, 1, f);
    fclose(f);

    return 1;
}

int file_save(const char* path, void* data, size_t size) {

    if (!path)
        return 0;

    char* new_path = NULL;
    size_t new_length = 0;

#if defined(ANDROID)

    size_t parameter_path_length = strlen(path);
    new_length = parameter_path_length + sizeof(ABSOLUTE_WD_PATH);
    new_path = (char*)malloc(new_length);

    memcpy(new_path, ABSOLUTE_WD_PATH, sizeof(ABSOLUTE_WD_PATH));
    memcpy(new_path + sizeof(ABSOLUTE_WD_PATH) - sizeof(char), path, parameter_path_length + 1);

#else

    new_length = strlen(path) + sizeof(path[0]);
    if ((new_path = (char*)malloc(new_length * sizeof(char))) == NULL) {
		LOGE("Unable to allocate memory.\n");
        return 0;
    }

    memcpy(new_path, path, new_length * sizeof(char));

#endif

    FILE* f = fopen(new_path, "wb"); if(!f) {
        LOGE("Error: opening file \"%s\" for write failed.\n", new_path);
        return 0;
    }

#ifdef DEBUG
    LOGI("writing to file \"%s\".\n", path);
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

#define ABSOLUTE_WD_PATH "data/data/com.pachuch.linhop/files/"

int file_load_asset(struct file* file, const char* path) {

    if (!spige_instance->asset_mgr)
        return 0;

    AAsset* asset = AAssetManager_open(spige_instance->asset_mgr, path, AASSET_MODE_BUFFER);

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
        LOGE("ERROR: Failed to read file to string, asset not found. (file: %s)\n", path);
        return 0;
    }
}

#elif defined (WIN32) || defined (_WIN32)
#include <Windows.h>

void spige_show_message(const char* fmt, ...) {

    size_t size;
    va_list args, tmp_args;
    char* data = 0;

    va_start(args, fmt);
    va_copy(tmp_args, args);
    size = _vscprintf(fmt, tmp_args) + 1;
    va_end(tmp_args);

    if (size > 0) {

        if ((data = (char*)malloc(size)) == NULL) {
			MessageBoxA(NULL, data, "Unable to allocate memory.", MB_ICONERROR | MB_OK);
            return;
        }

        if (vsnprintf_s(data, size, _TRUNCATE, fmt, args) < 0) {
            data[size - 1] = 0;
        }

        MessageBoxA(NULL, data, "Message", MB_OK);
        free(data);
    }

    va_end(args);
}

int file_load_asset(struct file* file, const char* path) {
    return file_load(file, path);
}

#else

int file_load_asset(struct file* file, const char* path) {
    return file_load(file, path);
}

#endif

