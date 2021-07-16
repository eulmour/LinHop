file(GLOB LinHop_SRC CONFIGURE_DEPENDS "*.h" "*.cpp")

set(CMAKE_CXX_STANDARD 20)
add_definitions(-DGLEW_STATIC)
set(GLEW_STATIC_LIBRARIES TRUE)

set(TARGET_NAME "LinHop")

set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/${TARGET_NAME}/bin")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_executable(${TARGET_NAME} ${LinHop_SRC})
target_link_libraries(${TARGET_NAME}
	glew32s
	glfw3
	opengl32
	User32
	Gdi32
	Shell32
	glm
	stb_image
)
