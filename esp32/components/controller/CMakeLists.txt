cmake_minimum_required(VERSION 3.28.3)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CONTROLLER_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../controller)
# file(GLOB_RECURSE SOURCES "${CONTROLLER_SOURCE_DIR}/src/**/*.cpp")

idf_component_register(
  # SRCS ${SOURCES} INCLUDE_DIRS "${CONTROLLER_SOURCE_DIR}/src/RequestHandler"
  # "${CONTROLLER_SOURCE_DIR}/src/Simulator"
  # "${CONTROLLER_SOURCE_DIR}/src/Communication"
  # "${CONTROLLER_SOURCE_DIR}/src/Obd2Protocol"
)

include(ExternalProject)

ExternalProject_Add(
  controller_proj
  SOURCE_DIR ${CONTROLLER_SOURCE_DIR}
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}
  CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
             -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  INSTALL_COMMAND ""
  BUILD_BYPRODUCTS "libcontroller.a"
  USES_TERMINAL_CONFIGURE TRUE
  USES_TERMINAL_BUILD TRUE
  USES_TERMINAL_INSTALL TRUE)

add_prebuilt_library(controller_lib
                     "${CMAKE_CURRENT_BINARY_DIR}/libcontroller.a")

target_include_directories(
  controller_lib
  INTERFACE "${CONTROLLER_SOURCE_DIR}/src/RequestHandler"
            "${CONTROLLER_SOURCE_DIR}/src/Simulator"
            "${CONTROLLER_SOURCE_DIR}/src/Communication"
            "${CONTROLLER_SOURCE_DIR}/src/Obd2Protocol")

add_dependencies(controller_lib controller_proj)

target_link_libraries(${COMPONENT_LIB} INTERFACE controller_lib)
