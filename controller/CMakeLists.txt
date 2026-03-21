cmake_minimum_required(VERSION 3.28.3)

project(controller)
add_library(controller STATIC)

target_include_directories(
  controller PUBLIC src src/RequestHandler src/Simulator src/Communication
  src/Obd2Protocol src/Utils)

file(GLOB_RECURSE SOURCES src/RequestHandler/*.cxx src/Simulator/*.cxx src/Communication/*.cxx
  src/Obd2Protocol/*.cxx)
target_sources(controller PRIVATE ${SOURCES})

target_compile_features(controller PUBLIC cxx_std_20)
