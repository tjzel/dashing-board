cmake_minimum_required(VERSION 3.28)
project(Controller)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(
        controller
        src/main.cpp
        src/parser.cpp
        src/DataFrame.cpp
        src/DataFrame.hpp
        src/RequestHandler.cpp
        src/RequestHandler.hpp
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