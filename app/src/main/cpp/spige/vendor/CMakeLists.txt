macro(SET_OPTION option value)
  set(${option} ${value} CACHE INTERNAL "" FORCE)
endmacro()

if (CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations -Wno-error=deprecated-declarations -Wno-language-extension-token -Wno-error=language-extension-token")
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
endif()

add_subdirectory ("freetype")
add_subdirectory ("stb_image")

option(CGLM_SHARED "Shared build" OFF)
option(CGLM_STATIC "Static build" ON)
add_subdirectory ("cglm")

if (ANDROID)
	add_subdirectory ("android")
else()
	add_subdirectory ("glew")
	add_subdirectory ("glfw")

	SET_OPTION(BUILD_STATIC_LIBS ON)
	SET_OPTION(BUILD_DYNAMIC_LIBS OFF)
	SET_OPTION(BUILD_EXAMPLE_PROGRAMS OFF)
	SET_OPTION(BUILD_TESTS OFF)
	SET_OPTION(ENABLE_WASAPI ON)
	SET_OPTION(ENABLE_JACK OFF)
	SET_OPTION(ENABLE_PULSEAUDIO ON)
	SET_OPTION(ENABLE_ALSA OFF)
	SET_OPTION(ENABLE_COREAUDIO ON)

	add_subdirectory ("soundio")
endif()