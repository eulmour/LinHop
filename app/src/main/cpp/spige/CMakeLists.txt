#add_definitions (-DGLEW_STATIC)

project("spige" C)

add_definitions (-DSPIGE_USE_EGL)
add_definitions (-DSPIGE_USE_GLES_3)


file(GLOB FILES_SRC CONFIGURE_DEPENDS "*.h" "src/*.h" "src/*.c")

#add_library(spige STATIC ${FILES_SRC})
add_library(spige SHARED ${FILES_SRC})
set_target_properties(spige PROPERTIES LINKER_LANGUAGE C)

add_subdirectory("vendor")

target_include_directories(spige PRIVATE
    ${ANDROID_NDK}/sources/android/native_app_glue
    ${CMAKE_CURRENT_LIST_DIR}/vendor/cglm/include
    ${CMAKE_CURRENT_LIST_DIR}/vendor/stb_image
    ${CMAKE_CURRENT_LIST_DIR}/vendor/freetype/include
)
# linux
#target_link_libraries (spige GL OpenGL glut glm)

# android
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
