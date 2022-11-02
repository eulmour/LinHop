#ifndef ENGINE_FRAMEWORK_H
#define ENGINE_FRAMEWORK_H

#include <array>

#define ENGINE_POINTER_COUNT_MAX 16

#if defined(__ANDROID__) || defined(ANDROID)
#   include <GLES3/gl31.h>
#   include <android/log.h>
#   include <android_native_app_glue.h>
#   include <android/sensor.h>
#   include <android/native_activity.h>

#   define LOGV_PRINT(fmt, ...)\
        ((void)__android_log_print(ANDROID_LOG_VERBOSE, "threaded_app", "[VERBOSE] " fmt, ##__VA_ARGS__))
#   define LOGI_PRINT(fmt, ...)\
        ((void)__android_log_print(ANDROID_LOG_INFO, "threaded_app", "[INFO] " fmt, ##__VA_ARGS__))
#   define LOGW_PRINT(fmt, ...)\
        ((void)__android_log_print(ANDROID_LOG_WARN, "threaded_app", "[WARNING] " fmt, ##__VA_ARGS__))
#   define LOGE_PRINT(fmt, ...)\
        ((void)__android_log_print(ANDROID_LOG_ERROR, "threaded_app", "[ERROR] " fmt, ##__VA_ARGS__))

#   define LOGV_WRITE LOGV_PRINT
#   define LOGI_WRITE LOGI_PRINT
#   define LOGW_WRITE LOGW_PRINT
#   define LOGE_WRITE LOGE_PRINT

#elif defined (WIN32) || defined (_WIN32)
#   define strncasecmp(s1, s2, n) _strnicmp(s1, s2, n)
#   define LOGV_PRINT(fmt, ...) ((void)printf("[VERBOSE] " fmt, ##__VA_ARGS__))
#   define LOGI_PRINT(fmt, ...) ((void)printf("[INFO] " fmt, ##__VA_ARGS__))
#   define LOGW_PRINT(fmt, ...) ((void)printf("[WARNING] " fmt, ##__VA_ARGS__))
#   define LOGE_PRINT(fmt, ...) ((void)fprintf(stderr, "[ERROR] " fmt, ##__VA_ARGS__))

#   define LOGV_WRITE(fmt, ...) engine_log_message("[VERBOSE] " fmt, ##__VA_ARGS__)
#   define LOGI_WRITE(fmt, ...) engine_log_message("[INFO] " fmt, ##__VA_ARGS__)
#   define LOGW_WRITE(fmt, ...)\
        engine_log_message("[WARNING] %s\n" fmt "File %s, line %d\n", strerror(errno), ##__VA_ARGS__, __FILE__, __LINE__)
#   define LOGE_WRITE(fmt, ...)\
        engine_log_message("[ERROR] %s\n" fmt "File %s, line %d\n", strerror(errno), ##__VA_ARGS__, __FILE__, __LINE__)
#else
#   define LOGV_PRINT(fmt, ...) ((void)printf("[VERBOSE] " fmt, ##__VA_ARGS__))
#   define LOGI_PRINT(fmt, ...) ((void)printf("[INFO] " fmt, ##__VA_ARGS__))
#   define LOGW_PRINT(fmt, ...) ((void)printf("[WARNING] " fmt, ##__VA_ARGS__))
#   define LOGE_PRINT(fmt, ...) ((void)fprintf(stderr, "[ERROR] " fmt, ##__VA_ARGS__))

#   define LOGV_WRITE(fmt, ...) engine_log_message("[VERBOSE] " fmt, ##__VA_ARGS__)
#   define LOGI_WRITE(fmt, ...) engine_log_message("[INFO] " fmt, ##__VA_ARGS__)
#   define LOGW_WRITE(fmt, ...)\
        engine_log_message("[WARNING] %s\n" fmt "File %s, line %d\n", strerror(errno), ##__VA_ARGS__, __FILE__, __LINE__)
#   define LOGE_WRITE(fmt, ...)\
        engine_log_message("[ERROR] %s\n" fmt "File %s, line %d\n", strerror(errno), ##__VA_ARGS__, __FILE__, __LINE__)
#endif

#ifdef ENGINE_WRITE_LOGS
#   ifndef NDEBUG
#       define LOGV LOGV_WRITE
#   else
#       define LOGV(...)  ((void)0)
#   endif
#   define LOGI LOGI_WRITE
#   define LOGW LOGW_WRITE
#   define LOGE LOGE_WRITE
#else
#   ifndef NDEBUG
#       define LOGV LOGV_PRINT
#   else
#       define LOGV(...)  ((void)0)
#   endif
#   define LOGI LOGI_PRINT
#   define LOGW LOGW_PRINT
#   define LOGE LOGE_PRINT
#endif

using Color = std::array<float, 4>;
using Vec2 = std::array<float, 2>;
using Vec3 = std::array<float, 3>;
using Vec4 = std::array<float, 4>;
using IVec2 = std::array<int, 2>;
using Mat4 = std::array<std::array<float, 4>, 4>;

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

// class Engine {
//     int width;
//     int height;

// #ifdef ENGINE_WRITE_LOGS
//     FILE* log;
// #endif

// #if defined(__ANDROID__) || defined(ANDROID)
//     ASensorVector acceleration;
//     AAssetManager* asset_mgr;
// #endif
// };

#ifdef __cplusplus
extern "C" {
#endif

// engine api
// int engine_init(Engine *app);
// void engine_viewport(Engine *app, int w, int h);
// void engine_destroy(Engine *app);
void engine_log_message(const char* fmt, ...);
int engine_get_cwd(char* buf, size_t max_size);
void engine_check_error();

unsigned int create_shader(unsigned int shader_type, const char *src);
unsigned int create_program(const char *vertex_src, const char *fragment_src);

unsigned int texture_load(const char *path);
void texture_unload(unsigned int id);

void set_uniform_mat4(unsigned int program, const char *name, float* value);
void set_uniform4f(unsigned int program, const char *name, const Vec4 value);

int file_load(struct file *file, const char *path);
int file_load_asset(struct file *file, const char *path);
int file_save(const char* path, void* data, size_t size);
int file_remove(const char* path);
void file_unload(struct file *file);

#ifdef __cplusplus
}
#endif

#endif //ENGINE_FRAMEWORK_H