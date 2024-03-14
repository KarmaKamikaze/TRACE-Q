cmake_minimum_required(VERSION 3.28)
project(TRACE_Q)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-DSQLITE_ENABLE_RTREE=1)

add_executable(TRACE_Q src/main.cpp external/sqlite/sqlite3.c
        src/trajectory_data_handling/trajectory_file_manager.cpp
        src/trajectory_data_handling/sqlite_querying.hpp
        src/trajectory_data_handling/trajectory_sql.hpp
        src/data/trajectory_structure.hpp
        src/trajectory_data_handling/trajectory_file_manager.hpp
        src/trajectory_data_handling/trajectory_sql.cpp
        src/trajectory_data_handling/sqlite_querying.cpp
        src/main.cpp
        src/trajectory_data_handling/sqlite_querying.cpp)