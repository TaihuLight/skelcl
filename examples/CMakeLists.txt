# set include dir for all examples
include_directories ("${PROJECT_SOURCE_DIR}/include")

include_directories (${SKELCL_COMMON_INCLUDE_DIR})
link_directories (${SKELCL_COMMON_LIB_DIR})

add_subdirectory (dot_product)
add_subdirectory (gauss)
add_subdirectory (mandelbrot)
add_subdirectory (matrix_mult)
add_subdirectory (montecarlo)
add_subdirectory (saxpy)
