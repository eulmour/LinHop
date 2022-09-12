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

#set_target_properties(spige PROPERTIES LINKER_LANGUAGE C)

add_subdirectory("vendor")

target_include_directories(spige PRIVATE
    ${ANDROID_NDK}/sources/android/native_app_glue
    ${CMAKE_CURRENT_LIST_DIR}/vendor/cglm/include
    ${CMAKE_CURRENT_LIST_DIR}/vendor/stb_image
    ${CMAKE_CURRENT_LIST_DIR}/vendor/freetype/include
    ${CMAKE_CURRENT_LIST_DIR}/vendor/soundio/soundio
    ${CMAKE_CURRENT_LIST_DIR}/vendor/glew
    ${CMAKE_CURRENT_LIST_DIR}/vendor/glwf/include
)

if (ANDROID)
    target_link_libraries(spige
        EGL
        GLESv1_CM
        GLESv3
        OpenSLES
        cglm
        stb_image
        freetype
        android
        log
    )
elseif(WIN32)
    target_link_libraries(spige
        opengl32
        glfw
        glew
        cglm
        stb_image
        freetype
		libsoundio_static
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
		libsoundio_static
        pulse
    )
endif()