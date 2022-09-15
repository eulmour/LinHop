cmake_minimum_required(VERSION 3.8)

project("linhop" C CXX)

set (CMAKE_CXX_STANDARD 17)

# add_definitions(-DSPIGE_WRITE_LOGS)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_CRT_NONSTDC_NO_WARNINGS)

add_subdirectory("spige")
add_subdirectory("game/vendor")

file (GLOB linhop_src CONFIGURE_DEPENDS "game/*.hpp" "game/*.cpp")

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
    ${CMAKE_SOURCE_DIR}/spige
    ${CMAKE_SOURCE_DIR}/spige/vendor/glew # TODO: maybe consider moving it to engine or leave freedom
    ${CMAKE_SOURCE_DIR}/game/vendor/glm/glm
)

target_link_libraries(linhop spige glm)

 # Android target
add_library(linhop_android SHARED Main.hpp Main.cpp ${linhop_src})

target_include_directories(linhop_android PRIVATE
    ${CMAKE_SOURCE_DIR}/spige
	${CMAKE_SOURCE_DIR}/spige/vendor/android
    ${CMAKE_SOURCE_DIR}/game/vendor/glm
)

target_link_libraries(linhop_android
    spige
    android
    log
)

target_compile_options(linhop PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
    -Werror -Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:
    /W3>)

target_compile_options(linhop_android PRIVATE -Werror -Wall -Wextra)