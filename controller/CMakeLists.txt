project(controller)
add_library(controller SHARED)

target_include_directories(controller PUBLIC src src/RequestHandler src/Simulator src/Communication src/Obd2Protocol)
file(GLOB_RECURSE SOURCES src/**/*.cpp)
target_sources(controller PRIVATE ${SOURCES})

# set_target_properties(controller PROPERTIES LIBRARY_OUTPUT_DIRECTORY build
# OUTPUT_NAME controller)
target_compile_features(controller PUBLIC cxx_std_20)

add_subdirectory(tests/controller)
