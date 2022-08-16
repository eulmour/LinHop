project("spige" C)

add_definitions(-DGLEW_STATIC)

file(GLOB FILES_SRC CONFIGURE_DEPENDS "*.h" "src/*.h" "src/*.c")

if (ANDROID)
    # add_definitions (-DSPIGE_USE_EGL)
    # add_definitions (-DSPIGE_USE_GLES_3)
    add_library(spige SHARED ${FILES_SRC})
else()
    #add_definitions (-DGLEW_STATIC)
    add_library(spige STATIC ${FILES_SRC})
endif()

set_target_properties(spige PROPERTIES LINKER_LANGUAGE C)

add_subdirectory("vendor")

target_include_directories(spige PRIVATE
    ${ANDROID_NDK}/sources/android/native_app_glue
    ${CMAKE_CURRENT_LIST_DIR}/vendor/cglm/include
    ${CMAKE_CURRENT_LIST_DIR}/vendor/stb_image
    ${CMAKE_CURRENT_LIST_DIR}/vendor/freetype/include
    ${CMAKE_CURRENT_LIST_DIR}/vendor/glew
    ${CMAKE_CURRENT_LIST_DIR}/vendor/glwf/include
)

if (ANDROID)
    target_link_libraries(spige
        egl
        glesv1_cm
        glesv3
        opensles
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
        # libsoundio
    )
else()
    target_link_libraries(spige
        GL
        OpenGL
        glfw
        glew
        cglm
        stb_image
        freetype
        # libsoundio
    )
endif()