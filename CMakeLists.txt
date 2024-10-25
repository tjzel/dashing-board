cmake_minimum_required(VERSION 3.30)
project(dashing-board)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")

add_subdirectory(controller)
