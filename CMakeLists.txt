cmake_minimum_required(VERSION 3.16)
project(structure C)

set(CMAKE_C_STANDARD 11)


include_directories(common)

add_subdirectory(map)
add_subdirectory(set)
add_subdirectory(concurrent)
add_subdirectory(expression)