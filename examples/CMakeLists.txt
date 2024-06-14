add_library(r2d_example_app INTERFACE)
target_link_libraries(r2d_example_app INTERFACE r2d SDL2-static)
target_include_directories(r2d_example_app INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_sources(r2d_example_app INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/example_app_sdl2.h")

add_executable(r2d_example_aa_raster aa_raster.cpp)
target_link_libraries(r2d_example_aa_raster r2d_example_app)