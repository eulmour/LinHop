project("spige" CXX)

set (CMAKE_CXX_STANDARD 17)

add_definitions(-DGLEW_STATIC)
add_definitions(-DSOUNDIO_STATIC_LIBRARY)

file(GLOB FILES_SRC CONFIGURE_DEPENDS "*.h" "src/*.h" "src/*cpp")

if (ANDROID)
    add_library(spige SHARED ${FILES_SRC})
else()
    add_library(spige STATIC ${FILES_SRC})
endif()

add_subdirectory("vendor")

target_include_directories(spige PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/vendor/android
    ${CMAKE_CURRENT_LIST_DIR}/vendor/cglm/include
    ${CMAKE_CURRENT_LIST_DIR}/vendor/stb_image
    ${CMAKE_CURRENT_LIST_DIR}/vendor/freetype/include
    ${CMAKE_CURRENT_LIST_DIR}/vendor/miniaudio
    ${CMAKE_CURRENT_LIST_DIR}/vendor/glew
    ${CMAKE_CURRENT_LIST_DIR}/vendor/glwf/include
)

if (ANDROID)
    target_link_libraries(spige
        EGL
        GLESv1_CM
        GLESv3
        OpenSLES
        miniaudio
        cglm
        stb_image
        freetype
        android
        log
        native_app_glue
    )
elseif(WIN32)
    target_link_libraries(spige
        opengl32
        glfw
        glew
        cglm
        stb_image
        freetype
        miniaudio
    )
elseif(UNIX AND NOT APPLE)
    target_link_libraries(spige
        GL
        OpenGL
        glfw
        glew
        cglm
        stb_image
        freetype
        miniaudio
        pulse
    )
endif()