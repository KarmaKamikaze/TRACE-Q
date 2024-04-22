#include <doctest/doctest.h>
#include "../simp-algorithms/TRACE_Q.hpp"
#include "../trajectory_data_handling/Trajectory_Manager.hpp"
#include "../trajectory_data_handling/File_Manager.hpp"
#include "Benchmark.hpp"


TEST_CASE("TRACE-Q runtime benchmarking") {

    SUBCASE("aS") {
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();

        auto resolution_scale = 2.0;
        auto min_query_accuracy = 0.95;


        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_query_accuracy, analytics::Benchmark::grid_density,
                                        analytics::Benchmark::grid_density, analytics::Benchmark::windows_per_grid_point,
                                        analytics::Benchmark::window_expansion_rate, analytics::Benchmark::time_interval,
                                        analytics::Benchmark::time_interval, analytics::Benchmark::knn_k};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy();

        std::cout << "Runtime: " << time / 1000 << " s\n";
        std::cout << "Query accuracy: " << query_accuracy << std::endl;
    }
}
