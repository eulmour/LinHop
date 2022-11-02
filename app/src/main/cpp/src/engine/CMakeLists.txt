project("engine" CXX)

set (CMAKE_CXX_STANDARD 17)

add_definitions(-DGLEW_STATIC)
add_definitions(-DSOUNDIO_STATIC_LIBRARY)

file(GLOB FILES_SRC CONFIGURE_DEPENDS "*.h" "src/*.h" "src/*cpp")

if (ANDROID)
    add_library(engine SHARED ${FILES_SRC})
else()
    add_library(engine STATIC ${FILES_SRC})
endif()

add_subdirectory("vendor")

target_include_directories(engine PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/vendor/android
    ${CMAKE_CURRENT_LIST_DIR}/vendor/stb_image
    ${CMAKE_CURRENT_LIST_DIR}/vendor/freetype/include
    ${CMAKE_CURRENT_LIST_DIR}/vendor/miniaudio
    ${CMAKE_CURRENT_LIST_DIR}/vendor/glew
    ${CMAKE_CURRENT_LIST_DIR}/vendor/glwf/include
    ${CMAKE_CURRENT_LIST_DIR}/vendor/glm
)

if (ANDROID)
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
elseif(UNIX AND NOT APPLE)
    target_link_libraries(engine
        GL
        OpenGL
        glfw
        glew
        stb_image
        freetype
        glm
        miniaudio
        pulse
    )
endif()