cmake_minimum_required(VERSION 3.28)
# project(ControllerTest)

# Temporarily.
set(CMAKE_BUILD_TYPE Debug)

if(NOT CMAKE_C_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_C_COMPILER clang)
endif()
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_COMPILER clang++)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(controller-test main.cpp)

target_link_libraries(controller-test controller)

set_target_properties(controller-test PROPERTIES RUNTIME_OUTPUT_DIRECTORY build
                                                 OUTPUT_NAME controller-test)
