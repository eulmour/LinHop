#ifndef SPIGE_FRAMEWORK_H
#define SPIGE_FRAMEWORK_H

#include "cglm/cglm.h"
//#include "cglm/struct.h"

#define SPIGE_POINTER_COUNT_MAX 16

#if defined(__ANDROID__) || defined(ANDROID)

    #include <GLES3/gl31.h>
    #include <android_native_app_glue.h>
    #include <android/log.h>
    #include <android/sensor.h>

    #define LOGI_PRINT(fmt, ...)\
        ((void)__android_log_print(ANDROID_LOG_INFO, "AndroidProject1.NativeActivity", "[INFO] " fmt, ##__VA_ARGS__))
    #define LOGW_PRINT(fmt, ...)\
        ((void)__android_log_print(ANDROID_LOG_WARN, "AndroidProject1.NativeActivity", "[WARNING] " fmt, ##__VA_ARGS__))
    #define LOGE_PRINT(fmt, ...)\
        ((void)__android_log_print(ANDROID_LOG_ERROR, "AndroidProject1.NativeActivity", "[ERROR] " fmt, ##__VA_ARGS__))

    #define LOGI_WRITE LOGI_PRINT
    #define LOGW_WRITE LOGW_PRINT
    #define LOGE_WRITE LOGE_PRINT

#elif defined (WIN32) || defined (_WIN32)

    #define strncasecmp(s1, s2, n) _strnicmp(s1, s2, n)

	#define LOGI_PRINT(fmt, ...) ((void)printf("[INFO] " fmt, ##__VA_ARGS__))
	#define LOGW_PRINT(fmt, ...) ((void)printf("[WARNING] " fmt, ##__VA_ARGS__))
	#define LOGE_PRINT(fmt, ...) ((void)fprintf(stderr, "[ERROR] " fmt, ##__VA_ARGS__))
	#define LOGI_WRITE(fmt, ...) spige_log_message("[INFO] " fmt, ##__VA_ARGS__)
    #define LOGW_WRITE(fmt, ...)\
        spige_log_message("[WARNING] %s\n" fmt "File %s, line %d\n", strerror(errno), ##__VA_ARGS__, __FILE__, __LINE__)
    #define LOGE_WRITE(fmt, ...)\
        spige_log_message("[ERROR] %s\n" fmt "File %s, line %d\n", strerror(errno), ##__VA_ARGS__, __FILE__, __LINE__)

#else

	#define LOGI_PRINT(fmt, ...) ((void)printf("[INFO] " fmt, ##__VA_ARGS__))
	#define LOGW_PRINT(fmt, ...) ((void)printf("[WARNING] " fmt, ##__VA_ARGS__))
	#define LOGE_PRINT(fmt, ...) ((void)fprintf(stderr, "[ERROR] " fmt, ##__VA_ARGS__))

	#define LOGI_WRITE(fmt, ...) spige_log_message("[INFO] " fmt, ##__VA_ARGS__)
    #define LOGW_WRITE(fmt, ...)\
        spige_log_message("[WARNING] %s\n" fmt "File %s, line %d\n", strerror(errno), ##__VA_ARGS__, __FILE__, __LINE__)
    #define LOGE_WRITE(fmt, ...)\
        spige_log_message("[ERROR] %s\n" fmt "File %s, line %d\n", strerror(errno), ##__VA_ARGS__, __FILE__, __LINE__)
#endif

#ifdef SPIGE_WRITE_LOGS
    #define LOGI LOGI_WRITE
    #define LOGW LOGW_WRITE
    #define LOGE LOGE_WRITE
#else
    #define LOGI LOGI_PRINT
    #define LOGW LOGW_PRINT
    #define LOGE LOGE_PRINT
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

#ifdef SPIGE_WRITE_LOGS
    FILE* log;
#endif

#if defined(__ANDROID__) || defined(ANDROID)
    ASensorVector acceleration;
    AAssetManager* asset_mgr;
#endif
};

#ifdef __cplusplus
extern "C" {
#endif

// engine api
int spige_init(struct spige *app);
void spige_viewport(struct spige *app, int w, int h);
void spige_destroy(struct spige *app);
void spige_log_message(const char* fmt, ...);
int spige_get_cwd(char* buf, size_t max_size);
void spige_check_error();

unsigned int create_shader(unsigned int shader_type, const char *src);
unsigned int create_program(const char *vertex_src, const char *fragment_src);

unsigned int texture_load(const char *path);
void texture_unload(unsigned int id);

void set_uniform_mat4(unsigned int program, const char *name, float* value);
void set_uniform4f(unsigned int program, const char *name, const vec4 value);

int file_load(struct file *file, const char *path);
int file_load_asset(struct file *file, const char *path);
int file_save(const char* path, void* data, size_t size);
int file_remove(const char* path);
void file_unload(struct file *file);

#ifdef __cplusplus
}
#endif

#endif //SPIGE_FRAMEWORK_H
