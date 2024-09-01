cmake_minimum_required(VERSION 3.28)
project(Controller)

# Temporarily.
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(controller SHARED src/DataFrame.cpp src/CommunicatorMock.cpp
                              src/Utils.cpp src/Message.cpp)

include_directories(src)
