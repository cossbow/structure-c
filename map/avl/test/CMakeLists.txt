project(test-avl C)

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 ")

add_executable(test-avl test-avl.c)
target_link_libraries(test-avl avl_tree)
