project(concurrent C)

aux_source_directory(implement src)
include_directories(interface)
add_library(concurrent STATIC ${src})

