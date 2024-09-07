project(controller-test)
add_executable(controller-test)

target_sources(controller-test PRIVATE main.cpp)
target_link_libraries(controller-test controller)

set_target_properties(controller-test PROPERTIES RUNTIME_OUTPUT_DIRECTORY build
  OUTPUT_NAME controller-test)
