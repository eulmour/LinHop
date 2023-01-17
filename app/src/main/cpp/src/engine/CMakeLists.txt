project("engine" CXX)

set (CMAKE_CXX_STANDARD 17)

add_definitions(-DGLEW_STATIC)
add_definitions(-DENGINE_WRITE_LOGS)

file(GLOB FILES_SRC CONFIGURE_DEPENDS "*.h" "src/*.h" "src/*cpp")

if (ANDROID)
    add_library(engine SHARED ${FILES_SRC})
else()
    add_library(engine STATIC ${FILES_SRC})
endif()

add_subdirectory("vendor")

target_include_directories(engine PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/vendor/stb_image
    ${CMAKE_CURRENT_LIST_DIR}/vendor/freetype/include
    ${CMAKE_CURRENT_LIST_DIR}/vendor/miniaudio
    ${CMAKE_CURRENT_LIST_DIR}/vendor/glm
)

if (NOT EMSCRIPTEN)
    target_include_directories(engine PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/vendor/glew
        ${CMAKE_CURRENT_LIST_DIR}/vendor/glwf/include
    )
endif()

if (ANDROID)
    target_include_directories(engine PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/vendor/android
    )
    target_link_libraries(engine
        EGL
        GLESv1_CM
        GLESv3
        OpenSLES
        miniaudio
        stb_image
        freetype
        glm
        android
        log
        native_app_glue
    )
elseif(WIN32)
    target_link_libraries(engine
        opengl32
        glfw
        glew
        stb_image
        freetype
        glm
        miniaudio
    )
elseif(EMSCRIPTEN)
    # target_link_libraries(linhop PUBLIC "-g")
    target_link_libraries(engine PUBLIC "-s ALLOW_MEMORY_GROWTH=1")
    target_link_libraries(engine PUBLIC "-s USE_GLFW=3")
    target_link_libraries(engine PUBLIC "-s USE_WEBGL2=1")
    target_link_libraries(engine PUBLIC "-s USE_FREETYPE=1")
    target_link_libraries(engine PUBLIC "-s STB_IMAGE=1")
    target_link_libraries(engine PUBLIC "-s WASM=1")
    target_link_libraries(engine PUBLIC "-fwasm-exceptions")
    target_link_libraries(engine PRIVATE glm miniaudio)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(engine
        GL
        glfw
        glew
        stb_image
        freetype
        glm
        miniaudio
        # pulse
    )
elseif(APPLE)
    find_library(OPENGL_LIB OpenGL)
    target_link_libraries(engine
        ${OPENGL_LIB}
        glfw
        glew
        stb_image
        freetype
        glm
        miniaudio
        # pulse
    )
endif()

if (MSVC)
    target_compile_options(engine PRIVATE /W4)
    # add_definitions(-D_DISABLE_VECTOR_ANNOTATION)
    # target_compile_options(engine PRIVATE /W4 /fsanitize=address /Zi)
else()
    target_compile_options(engine PRIVATE -Wall -Wextra -Wpedantic)
endif()

