#include <doctest/doctest.h>
#include "../simp-algorithms/TRACE_Q.hpp"
#include "../trajectory_data_handling/Trajectory_Manager.hpp"
#include "../trajectory_data_handling/File_Manager.hpp"
#include "Benchmark.hpp"


TEST_CASE("TRACE-Q KNN K benchmarking") {

    /*
     * The TRACE-Q benchmark tests assume that the dataset consist of 50 trajectories.
     */

    double resolution_scale = 2.0;
    double min_query_accuracy = 0.95;
    int max_trajectories_in_batch = 8;
    int max_threads = 50;
    auto range_query_grid_density = 0.1;
    auto knn_query_grid_density = 0.1;
    int windows_per_grid_point = 3;
    double window_expansion_rate = 1.3;
    double range_query_time_interval_multiplier = 0.1;
    double knn_query_time_interval_multiplier = 0.1;
    //int knn_k = 10;
    bool use_KNN_for_query_accuracy = true;

    SUBCASE("TRACE-Q Runtime vs Query Accuracy - KNN K = 1") {
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();

        int custom_knn_k = 1;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_query_accuracy, max_trajectories_in_batch, max_threads,
                                        range_query_grid_density,
                                        knn_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, range_query_time_interval_multiplier,
                                        knn_query_time_interval_multiplier, custom_knn_k,
                                        use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy();

        std::cout << "TRACE-Q Runtime vs Query Accuracy - KNN K = 1" << std::endl;
        std::cout << "Runtime: " << time / 1000 << " s\n";
        std::cout << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n"
                  << "KNN Query Accuracy: " << query_accuracy.knn_f1 << std::endl;
    }

    SUBCASE("TRACE-Q Runtime vs Query Accuracy - KNN K = 10") {
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();

        int custom_knn_k = 10;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_query_accuracy, max_trajectories_in_batch, max_threads,
                                        range_query_grid_density,
                                        knn_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, range_query_time_interval_multiplier,
                                        knn_query_time_interval_multiplier, custom_knn_k,
                                        use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy();

        std::cout << "TRACE-Q Runtime vs Query Accuracy - KNN K = 10" << std::endl;
        std::cout << "Runtime: " << time / 1000 << " s\n";
        std::cout << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n"
                  << "KNN Query Accuracy: " << query_accuracy.knn_f1 << std::endl;
    }

    SUBCASE("TRACE-Q Runtime vs Query Accuracy - KNN K = 50") {
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();

        int custom_knn_k = 50;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_query_accuracy, max_trajectories_in_batch, max_threads,
                                        range_query_grid_density,
                                        knn_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, range_query_time_interval_multiplier,
                                        knn_query_time_interval_multiplier, custom_knn_k,
                                        use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy();

        std::cout << "TRACE-Q Runtime vs Query Accuracy - KNN K = 50" << std::endl;
        std::cout << "Runtime: " << time / 1000 << " s\n";
        std::cout << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n"
                  << "KNN Query Accuracy: " << query_accuracy.knn_f1 << std::endl;
    }
}

TEST_CASE("TRACE-Q IS KNN NECESSARY?") {

    /*
     * The TRACE-Q benchmark tests assume that the dataset consist of 50 trajectories.
     */

    double resolution_scale = 2.0;
    double min_query_accuracy = 0.95;
    int max_trajectories_in_batch = 8;
    int max_threads = 50;
    auto range_query_grid_density = 0.1;
    auto knn_query_grid_density = 0.1;
    int windows_per_grid_point = 3;
    double window_expansion_rate = 1.3;
    double range_query_time_interval_multiplier = 0.1;
    double knn_query_time_interval_multiplier = 0.1;
    int knn_k = 10;
    //bool use_KNN_for_query_accuracy = true;

    SUBCASE("TRACE-Q WITH KNN") {
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();

        bool custom_use_KNN_for_query_accuracy = true;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_query_accuracy, max_trajectories_in_batch, max_threads,
                                        range_query_grid_density,
                                        knn_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, range_query_time_interval_multiplier,
                                        knn_query_time_interval_multiplier, knn_k,
                                        custom_use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy();

        std::cout << "TRACE-Q WITH KNN" << std::endl;
        std::cout << "Runtime: " << time / 1000 << " s\n";
        std::cout << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n"
                  << "KNN Query Accuracy: " << query_accuracy.knn_f1 << std::endl;
    }

    SUBCASE("TRACE-Q WITHOUT KNN") {
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();

        bool custom_use_KNN_for_query_accuracy = false;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_query_accuracy, max_trajectories_in_batch, max_threads,
                                        range_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, range_query_time_interval_multiplier,
                                        custom_use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy();

        std::cout << "TRACE-Q WITHOUT KNN" << std::endl;
        std::cout << "Runtime: " << time / 1000 << " s\n";
        std::cout << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n"
                  << "KNN Query Accuracy: " << query_accuracy.knn_f1 << std::endl;
    }
}

TEST_CASE("Query Accuracy") {
auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy();

std::cout << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n"
<< "KNN Query Accuracy: " << query_accuracy.knn_f1 << std::endl;
}