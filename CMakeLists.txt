cmake_minimum_required(VERSION 3.30)
project(dashing-board)

# Temporarily.
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_subdirectory(controller)
