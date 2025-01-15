cmake_minimum_required(VERSION 3.28.3)

project(server)

add_executable(server)
target_link_libraries(server controller)
target_include_directories(server PRIVATE src)
file(GLOB_RECURSE SERVER_SOURCES src/*.cpp)
target_sources(server PRIVATE ${SERVER_SOURCES})
target_compile_features(server PUBLIC cxx_std_20)
