add_executable(mrpa_test
        mrpa_test.cpp
        ../src/simp-algorithms/MRPA.hpp
        ../src/simp-algorithms/MRPA.cpp
)
target_link_libraries(mrpa_test PRIVATE doctest::doctest_with_main)

add_executable(trajectory_test trajectory_test.cpp)
target_link_libraries(trajectory_test PRIVATE doctest::doctest_with_main)

add_executable(node_test node_test.cpp)
target_link_libraries(node_test PRIVATE doctest::doctest_with_main)

add_executable(query_test
        query_test.cpp
        ../src/querying/Query.hpp
        ../src/querying/Query.cpp
)
target_link_libraries(query_test PRIVATE doctest::doctest_with_main)

add_executable(benchmark_test
        benchmark_test.cpp
)
target_link_libraries(benchmark_test PRIVATE doctest::doctest_with_main)

add_test(NAME mrpa_test COMMAND mrpa_test)
add_test(NAME trajectory_test COMMAND trajectory_test)
add_test(NAME node_test COMMAND node_test)
add_test(NAME query_test COMMAND query_test)
add_test(NAME benchmark_test COMMAND benchmark_test)