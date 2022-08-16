add_subdirectory ("freetype")
add_subdirectory ("stb_image")

option(CGLM_SHARED "Shared build" OFF)
option(CGLM_STATIC "Static build" ON)
add_subdirectory ("cglm")

if (NOT ANDROID)
    add_subdirectory ("glew")
    add_subdirectory ("glfw")
endif()