#ifndef ENGINE_FRAMEWORK_H
#define ENGINE_FRAMEWORK_H

#include <array>
#include <string.h>
#include <errno.h>

#define ENGINE_POINTER_COUNT_MAX 16

#if defined(__ANDROID__) || defined(ANDROID)
#   include <GLES3/gl31.h>
#   include <android/log.h>
#   include <android_native_app_glue.h>
#   include <android/sensor.h>
#   include <android/native_activity.h>
#   define LOGV_PRINT(fmt, ...)\
        ((void)__android_log_print(ANDROID_LOG_VERBOSE, "threaded_app", "[VERBOSE] " fmt "\n", ##__VA_ARGS__))
#   define LOGI_PRINT(fmt, ...)\
        ((void)__android_log_print(ANDROID_LOG_INFO, "threaded_app", "[INFO] " fmt "\n", ##__VA_ARGS__))
#   define LOGW_PRINT(fmt, ...)\
        ((void)__android_log_print(ANDROID_LOG_WARN, "threaded_app", "[WARNING] " fmt "\n", ##__VA_ARGS__))
#   define LOGE_PRINT(fmt, ...)\
        ((void)__android_log_print(ANDROID_LOG_ERROR, "threaded_app", "[ERROR] " fmt "\n", ##__VA_ARGS__))

#   define LOGV_WRITE LOGV_PRINT
#   define LOGI_WRITE LOGI_PRINT
#   define LOGW_WRITE LOGW_PRINT
#   define LOGE_WRITE LOGE_PRINT
#elif defined (WIN32) || defined (_WIN32)
#   define strncasecmp(s1, s2, n) _strnicmp(s1, s2, n)
#   define LOGV_PRINT(fmt, ...) ((void)printf("[VERBOSE] " fmt "\n", ##__VA_ARGS__))
#   define LOGI_PRINT(fmt, ...) ((void)printf("[INFO] " fmt "\n", ##__VA_ARGS__))
#   define LOGW_PRINT(fmt, ...) ((void)printf("[WARNING] " fmt "\n", ##__VA_ARGS__))
#   define LOGE_PRINT(fmt, ...) ((void)fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__))
#   define LOGV_WRITE(fmt, ...) engine_log_message("[VERBOSE] " fmt "\n", ##__VA_ARGS__)
#   define LOGI_WRITE(fmt, ...) engine_log_message("[INFO] " fmt "\n", ##__VA_ARGS__)
#   define LOGW_WRITE(fmt, ...)\
        engine_log_message("[WARNING] " fmt "\n[INFO] Reason: %s\n", ##__VA_ARGS__, errno != 0 ? strerror(errno) : "unknown")
#   define LOGE_WRITE(fmt, ...)\
        engine_log_message("[ERROR] " fmt "\n[INFO] Reason: %s, file %s, line %d\n", ##__VA_ARGS__, errno != 0 ? strerror(errno) : "unknown", __FILE__, __LINE__)
#else
#   define LOGV_PRINT(fmt, ...) ((void)printf("[VERBOSE] " fmt "\n", ##__VA_ARGS__))
#   define LOGI_PRINT(fmt, ...) ((void)printf("[INFO] " fmt "\n", ##__VA_ARGS__))
#   define LOGW_PRINT(fmt, ...) ((void)printf("[WARNING] " fmt "\n", ##__VA_ARGS__))
#   define LOGE_PRINT(fmt, ...) ((void)fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__))
#   define LOGV_WRITE(fmt, ...) engine_log_message("[VERBOSE] " fmt "\n", ##__VA_ARGS__)
#   define LOGI_WRITE(fmt, ...) engine_log_message("[INFO] " fmt "\n", ##__VA_ARGS__)
#   define LOGW_WRITE(fmt, ...)\
        engine_log_message("[WARNING] " fmt "\n[INFO] Reason: %s\n", ##__VA_ARGS__, errno != 0 ? strerror(errno) : "unknown")
#   define LOGE_WRITE(fmt, ...)\
        engine_log_message("[ERROR] " fmt "\n[INFO] Reason: %s, file %s, line %d\n", ##__VA_ARGS__, errno != 0 ? strerror(errno) : "unknown", __FILE__, __LINE__)
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
    std::size_t path_size;
    std::size_t size;
    void* data;
};

#ifdef __cplusplus
extern "C" {
#endif

void engine_log_message(const char* fmt, ...);
int engine_get_cwd(char* buf, std::size_t max_size);
void engine_check_error();

unsigned int create_shader(unsigned int shader_type, const char *src);
unsigned int create_program(const char *vertex_src, const char *fragment_src);

unsigned int texture_create(int width, int height, const void* data);
unsigned int texture_load_from_file(const char *path);
void texture_unload(unsigned int id);

int file_load(struct file *file, const char *path);
int file_load_asset(struct file *file, const char *path);
int file_save(const char* path, void* data, std::size_t size);
int file_remove(const char* path);
void file_unload(struct file *file);

#ifdef __cplusplus
}
#endif

#endif //ENGINE_FRAMEWORK_H
