project(test-map-avl C)

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 ")

add_executable(test-map-avl test-avl.c)
target_link_libraries(test-map-avl map-avl)
