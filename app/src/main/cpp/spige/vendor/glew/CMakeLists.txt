add_library(glew STATIC glew.c GL/glew.h GL/eglew.h GL/wglew.h GL/glxew.h)

# target_sources(spige
#     PRIVATE
#         ${glew_SRC}
# )