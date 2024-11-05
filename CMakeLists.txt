# CMakeList.txt : CMake project for r2dc, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  #cmake_policy(SET CMP0141 NEW)
  #set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("r2d")

include(cmake/CPM.cmake)

CPMAddPackage(
    NAME                SDL
    GITHUB_REPOSITORY   libsdl-org/SDL
    VERSION             2.28.3
    GIT_TAG             release-2.28.3
    OPTIONS             "SDL_SHARED OFF"
                        "SDL_TESTS OFF"
                        "SDL_TEST_LIBRARY OFF"
                        "SDL_DISABLE_INSTALL ON"
)

if (MSVC)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

# Add source to this project's executable.
add_library(stb_image_write "stb_stub.c" "stb_image_write.h")

add_library(r2d INTERFACE)
target_sources(r2d INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/r2d.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/r2d_core.hpp")
target_include_directories(r2d INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(r2d INTERFACE cxx_std_17)

add_subdirectory(examples)