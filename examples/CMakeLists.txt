add_library(r2d_example_lib STATIC
    "../r2d_surface.hpp"
    "example_app_sdl2.cpp"
    "example_app_sdl2.h")
target_link_libraries(r2d_example_lib PUBLIC r2d SDL2-static)
target_include_directories(r2d_example_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(r2d_example_aa_raster aa_raster.cpp)
target_link_libraries(r2d_example_aa_raster r2d_example_lib)

add_executable(r2d_example_polygon polygon.cpp)
target_link_libraries(r2d_example_polygon r2d_example_lib)