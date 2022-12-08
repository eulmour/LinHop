cmake_minimum_required(VERSION 3.8)

add_subdirectory("engine")
add_definitions(-DENGINE_WRITE_LOGS)

file (GLOB linhop_src CONFIGURE_DEPENDS "*.hpp" "*.cpp")

# Default target
if (WIN32)
	add_executable(linhop WIN32 Main.hpp Main.cpp ${linhop_src})
else()
	add_executable(linhop Main.hpp Main.cpp ${linhop_src})
endif()

if (ANDROID)
	# Export ANativeActivity_onCreate(),
	# Refer to: https://github.com/android-ndk/ndk/issues/381.
	set(CMAKE_SHARED_LINKER_FLAGS
			"${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
endif()

target_include_directories(linhop PRIVATE
    ${CMAKE_SOURCE_DIR}/src/engine/vendor/glew
    ${CMAKE_SOURCE_DIR}/src/engine/vendor/glm/glm
)

target_link_libraries(linhop engine glm)

 # Android target
add_library(linhop_android SHARED Main.hpp Main.cpp ${linhop_src})

target_include_directories(linhop_android PRIVATE
	${CMAKE_SOURCE_DIR}/src/engine/vendor/android
    ${CMAKE_SOURCE_DIR}/src/engine/vendor/glm
)

target_link_libraries(linhop_android
    engine
    android
    log
)

if (MSVC)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /W4)
    # target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /W4 /fsanitize=address /Zi)
else()
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

