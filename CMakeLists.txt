cmake_minimum_required(VERSION 3.28)
project(TRACE_Q)

set(CMAKE_CXX_STANDARD 20)

include(tests/doctest.cmake)

add_executable(TRACE_Q main.cpp datastructure/trajectory.cpp sqlite/sqlite3.c)
add_executable(trajectory_test tests/trajectory_test.cpp datastructure/trajectory.cpp)


target_link_libraries(trajectory_test PRIVATE doctest::doctest_with_main)
#target_link_libraries(sqlite/sqlite3.h SQLite::SQLite3)

enable_testing()
add_test(NAME trajectory_test COMMAND trajectory_test)