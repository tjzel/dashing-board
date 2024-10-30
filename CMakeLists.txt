cmake_minimum_required(VERSION 3.30)
project(dashing-board)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")

# set(CMAKE_BINARY_DIR build)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

# Define the output directories
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin) # For static libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Specify where to place all binary files (EXECUTABLEs and LIBRARYs)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_subdirectory(controller build)

# # Assuming the target name is 'controller', adjust if it has different name
# # get_target_property(controller controller LIBRARY_OUTPUT_DIRECTORY)
# set_target_properties(controller PROPERTIES
# LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
# ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
# RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
# )
