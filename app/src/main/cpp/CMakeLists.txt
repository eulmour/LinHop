cmake_minimum_required(VERSION 3.8)

project("linhop" C CXX)

set (CMAKE_CXX_STANDARD 17)

# add_definitions(-DLINHOP_WRITE_LOGS)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_CRT_NONSTDC_NO_WARNINGS)

add_subdirectory("src")