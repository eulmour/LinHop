#ifndef ENGINE_H
#define ENGINE_H

#include "src/Framework.h"
#include "src/Audio.h"
#include "src/Object.h"
#include "src/Engine.h"

#ifndef __cplusplus
#   error "C API is not available"
#endif

#if defined(WIN32) || defined(_WIN32)
#   define NOMINMAX
#   include <Windows.h>
#   define ENGINE_ENTRY(MainApplication) \
        int APIENTRY WinMain(_In_ HINSTANCE hInstance, \
                             _In_opt_ HINSTANCE hPrevInstance, \
                             _In_ LPTSTR lpCmdLine, \
                             _In_ int nCmdShow) { \
            (void)hInstance, (void)hPrevInstance, (void)lpCmdLine, (void)nCmdShow; \
            auto* app = new MainApplication(); \
            auto* engine = new Engine(*app, __argc, __argv); \
            engine->run(); \
            delete engine; \
            delete app; \
        }
#elif defined(__ANDROID__) || defined(ANDROID)
#   define ENGINE_ENTRY(MainApplication) \
        void android_main(struct android_app* state) { \
            auto* app = new MainApplication(); \
            auto* engine = new Engine(*app, state); \
            engine->run(); \
            delete engine; \
            delete app; \
        }
#else
#   define ENGINE_ENTRY(MainApplication) \
        int main(int argc, char *argv[]) { \
            auto* app = new MainApplication(); \
            auto* engine = new Engine(*app, argc, argv); \
            engine->run(); \
            delete engine; \
            delete app; \
        }
#endif

#endif //ENGINE_H
