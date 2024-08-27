cmake_minimum_required(VERSION 3.28)
project(Controller)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(
        controller
        src/main.cpp
        src/DataFrame.cpp
        src/CommunicatorMock.cpp
        src/Utils.cpp
)

set_target_properties(
        controller
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY
        bin
        OUTPUT_NAME
        controller
)

include_directories(src)

target_link_libraries(controller)