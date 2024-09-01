cmake_minimum_required(VERSION 3.28)
project(Controller)

# Temporarily.
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(controller-test main.cpp)

add_subdirectory(../../controller ../../controller/bin)

target_link_libraries(controller-test controller)

set_target_properties(controller-test PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin
                                                 OUTPUT_NAME controller-test)

include_directories(./ ../../controller/src)
