#ifndef SPIGE_FRAMEWORK_H
#define SPIGE_FRAMEWORK_H

#include <EGL/egl.h>
#include <GLES3/gl31.h>
#include "cglm/cglm.h"

#define SPIGE_POINTER_COUNT_MAX 16

#if defined(__ANDROID__) || defined(ANDROID)
#include <android_native_app_glue.h>
#include <android/log.h>
#include <android/sensor.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "AndroidProject1.NativeActivity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "AndroidProject1.NativeActivity", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "AndroidProject1.NativeActivity", __VA_ARGS__))
#else
#define LOGI(...) ((void)printf(__VA_ARGS__))
#define LOGW(...) ((void)printf(__VA_ARGS__))
#define LOGE(...) ((void)fprintf(stderr, __VA_ARGS__))
#endif

enum state {
    STATE_OFF,
    STATE_ERROR,
    STATE_READY,
    STATE_BUSY
};

struct file {
    char* path;
    size_t path_size;
    size_t size;
    void* data;
};

struct spige {
    int width;
    int height;
    float cursor[SPIGE_POINTER_COUNT_MAX][2]; // pointer count

#if defined(__ANDROID__) || defined(ANDROID)
    ASensorVector acceleration;
    AAssetManager* asset_mgr;
#endif
};

#ifdef __cplusplus
extern "C" {
#endif

// engine api
bool spige_init(struct spige *app);
void spige_viewport(struct spige *app, int w, int h);
void spige_destroy(struct spige *app);
void check_error();
GLuint create_shader(GLenum shader_type, const char *src);
GLuint create_program(const char *vertex_src, const char *fragment_src);
GLuint texture_load(const char *path);
void texture_unload(GLuint id);
void set_uniform_mat4(GLuint program, const char *name, GLfloat *value);
void set_uniform4f(GLuint program, const char *name, const vec4 value);
int file_load(struct file *file, const char *path);
int file_load_asset(struct file *file, const char *path);
int file_save(const char* path, void* data, size_t size);
int file_remove(const char* path);
void file_unload(struct file *file);

#ifdef __cplusplus
}
#endif

#endif //SPIGE_FRAMEWORK_H
