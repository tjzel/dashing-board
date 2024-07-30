cmake_minimum_required(VERSION 3.28)
project(Controller)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        controller
        src/requests.h
        src/main.cpp
        src/parser.cpp
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

target_link_libraries(controller boost_system boost_iostreams boost_serialization)