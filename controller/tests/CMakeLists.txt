cmake_minimum_required(VERSION 3.28)

project(controller-test)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/d144031940543e15423a25ae5a8a74141044862f.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(controller-test)

target_sources(controller-test PRIVATE main.cpp)
target_link_libraries(controller-test controller GTest::gtest_main)
target_compile_features(controller-test PUBLIC cxx_std_20)
target_compile_options(controller-test PRIVATE "-Wno-unused-result")

include(GoogleTest)
gtest_discover_tests(controller-test)
