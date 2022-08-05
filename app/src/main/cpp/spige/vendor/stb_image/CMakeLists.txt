file(GLOB stb_image_SRC CONFIGURE_DEPENDS "*.h" "*.c")

add_library(stb_image STATIC ${stb_image_SRC})