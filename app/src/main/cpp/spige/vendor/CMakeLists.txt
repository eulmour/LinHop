add_subdirectory ("freetype")
add_subdirectory ("stb_image")

option(CGLM_SHARED "Shared build" OFF)
option(CGLM_STATIC "Static build" ON)
add_subdirectory ("cglm")
