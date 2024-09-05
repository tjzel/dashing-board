cmake_minimum_required(VERSION 3.28)

# Temporarily.
set(CMAKE_BUILD_TYPE Debug)

if(NOT CMAKE_C_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_C_COMPILER clang)
endif()
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_COMPILER clang++)
endif()

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_library(controller SHARED ${SOURCES})

#  TODO: Fix this path or something.
add_subdirectory(tests/controller)

set_target_properties(controller PROPERTIES LIBRARY_OUTPUT_DIRECTORY build
                                            OUTPUT_NAME controller)

target_include_directories(controller PUBLIC src)
