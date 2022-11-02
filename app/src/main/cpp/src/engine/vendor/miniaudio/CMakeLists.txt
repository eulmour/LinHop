file(GLOB miniaudio_SRC CONFIGURE_DEPENDS "*.h" "*.c")

add_library(miniaudio STATIC ${miniaudio_SRC})