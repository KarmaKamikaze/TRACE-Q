cmake_minimum_required(VERSION 3.28)
project(TRACE_Q)

set(CMAKE_CXX_STANDARD 20)

include(tests/doctest.cmake)

add_executable(TRACE_Q main.cpp datastructure/trajectory.cpp)
add_executable(trajectory_test tests/trajectory_test.cpp datastructure/trajectory.cpp)

target_link_libraries(trajectory_test PRIVATE doctest::doctest_with_main)

enable_testing()
add_test(NAME trajectory_test COMMAND trajectory_test)