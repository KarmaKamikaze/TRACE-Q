cmake_minimum_required(VERSION 3.28)

add_executable(TRACE_Q_Benchmark
        TRACE_Q_Benchmark.cpp
        ../simp-algorithms/TRACE_Q.hpp
        ../simp-algorithms/TRACE_Q.cpp
        ../trajectory_data_handling/Trajectory_Manager.hpp
        ../trajectory_data_handling/Trajectory_Manager.cpp
        ../trajectory_data_handling/File_Manager.hpp
        ../trajectory_data_handling/File_Manager.cpp
        ../simp-algorithms/MRPA.hpp
        ../simp-algorithms/MRPA.cpp
        ../querying/Range_Query_Test.hpp
        ../querying/Range_Query_Test.cpp
        ../querying/KNN_Query_Test.hpp
        ../querying/KNN_Query_Test.cpp
        ../querying/Range_Query.hpp
        ../querying/Range_Query.cpp
        ../querying/KNN_Query.hpp
        ../querying/KNN_Query.cpp
        Benchmark.hpp
        Benchmark.cpp
        F1.hpp
        F1.cpp
        benchmark-query-objects/Benchmark_Query.hpp
        benchmark-query-objects/Benchmark_Range_Query.hpp
        benchmark-query-objects/Benchmark_KNN_Query.hpp
)
target_link_libraries(TRACE_Q_Benchmark PRIVATE doctest::doctest_with_main "${PQXX_LIBRARIES}")

include(../../doctest.cmake)
include(../../libpqxx.cmake)


add_test(NAME TRACE_Q_Benchmark COMMAND TRACE_Q_Benchmark)
