cmake_minimum_required(VERSION 3.28.3)

project(controller)
add_library(controller STATIC)

target_include_directories(
  controller PUBLIC src src/RequestHandler src/Simulator src/Communication
  src/Obd2Protocol)

file(GLOB_RECURSE SOURCES src/RequestHandler/*.cpp src/Simulator/*.cpp src/Communication/*.cpp
  src/Obd2Protocol/*.cpp)
target_sources(controller PRIVATE ${SOURCES})

target_compile_features(controller PUBLIC cxx_std_20)

add_executable(server)
target_link_libraries(server controller)
target_include_directories(server PRIVATE src/Server)
file(GLOB_RECURSE SERVER_SOURCES src/Server/*.cpp)
target_sources(server PRIVATE ${SERVER_SOURCES})
target_compile_features(server PUBLIC cxx_std_20)
