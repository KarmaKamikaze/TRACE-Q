#include <doctest/doctest.h>
#include "../simp-algorithms/TRACE_Q.hpp"
#include "../trajectory_data_handling/Trajectory_Manager.hpp"
#include "../trajectory_data_handling/File_Manager.hpp"
#include "Benchmark.hpp"

auto logger = analytics::Benchmark::get_logger();
const auto query_objects = analytics::Benchmark::initialize_query_objects();

TEST_CASE("TRACE-Q IS KNN NECESSARY?") {

    /*
     * The TRACE-Q benchmark tests assume that the dataset consist of 50 trajectories.
     */

    trajectory_data_handling::Trajectory_Manager::reset_all_data();
    trajectory_data_handling::File_Manager::load_tdrive_dataset();

    double resolution_scale = 1.1;
    double min_range_query_accuracy = 0.95;
    //double min_knn_query_accuracy = 0.95;
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

        double custom_min_range_query_accuracy = 0.85;
        double custom_min_knn_query_accuracy = 0.88;
        bool custom_use_KNN_for_query_accuracy = true;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, custom_min_range_query_accuracy, custom_min_knn_query_accuracy,
                                        max_trajectories_in_batch, max_threads,
                                        range_query_grid_density,
                                        knn_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, range_query_time_interval_multiplier,
                                        knn_query_time_interval_multiplier, knn_k,
                                        custom_use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log;

        log << "TRACE-Q WITH KNN\n";
        log << "Parameters:\n";
        log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log << "Min Range Query Accuracy: " << std::to_string(custom_min_range_query_accuracy) << "\n";
        log << "Min KNN Query Accuracy: " << std::to_string(custom_min_knn_query_accuracy) << "\n";
        log << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log << "Max Threads: " << std::to_string(max_threads) << "\n";
        log << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log << "KNN K: " << std::to_string(knn_k) << "\n";
        log << "Use KNN For Query Accuracy: " << std::to_string(custom_use_KNN_for_query_accuracy) << "\n";
        log << "Benchmark:\n";
        log << "Runtime: " << time / 1000 << " s\n";
        log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
        log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
        log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();
    }

    SUBCASE("TRACE-Q WITHOUT KNN") {

        bool custom_use_KNN_for_query_accuracy = false;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, max_trajectories_in_batch, max_threads,
                                        range_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, range_query_time_interval_multiplier,
                                        custom_use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log;

        log << "TRACE-Q WITHOUT KNN\n";
        log << "Parameters:\n";
        log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log << "Max Threads: " << std::to_string(max_threads) << "\n";
        log << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log << "Use KNN For Query Accuracy: " << std::to_string(custom_use_KNN_for_query_accuracy) << "\n";
        log << "Benchmark:\n";
        log << "Runtime: " << time / 1000 << " s\n";
        log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
        log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
        log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << std::endl;
        logger << log.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();
    }

}

TEST_CASE("TRACE_Q Window expansion rate benchmarking") {

    trajectory_data_handling::Trajectory_Manager::reset_all_data();
    trajectory_data_handling::File_Manager::load_tdrive_dataset();

    double resolution_scale = 1.1;
    double min_range_query_accuracy = 0.95;
    double min_knn_query_accuracy = 0.95;
    int max_trajectories_in_batch = 8;
    int max_threads = 50;
    auto range_query_grid_density = 0.1;
    auto knn_query_grid_density = 0.1;
    int windows_per_grid_point = 4;
    //double window_expansion_rate = 1.3;
    double range_query_time_interval_multiplier = 0.1;
    double knn_query_time_interval_multiplier = 0.1;
    int knn_k = 10;
    bool use_KNN_for_query_accuracy = true;

    SUBCASE("TRACE-Q Runtime vs Query Accuracy - Window expansion rate = 1.2") {

        double custom_window_expansion_rate = 1.2;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                        max_trajectories_in_batch, max_threads,
                                        range_query_grid_density,
                                        knn_query_grid_density, windows_per_grid_point,
                                        custom_window_expansion_rate, range_query_time_interval_multiplier,
                                        knn_query_time_interval_multiplier, knn_k,
                                        use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log;

        log << "TRACE-Q Runtime vs Query Accuracy - Window expansion rate = 1.2\n";
        log << "Parameters:\n";
        log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log << "Max Threads: " << std::to_string(max_threads) << "\n";
        log << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log << "Window Expansion Rate: " << std::to_string(custom_window_expansion_rate) << "\n";
        log << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log << "KNN K: " << std::to_string(knn_k) << "\n";
        log << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log << "Benchmark:\n";
        log << "Runtime: " << time / 1000 << " s\n";
        log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
        log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
        log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();
    }

    SUBCASE("TRACE-Q Runtime vs Query Accuracy - Window expansion rate = 1.3") {

        double custom_window_expansion_rate = 1.3;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                        max_trajectories_in_batch, max_threads,
                                        range_query_grid_density,
                                        knn_query_grid_density, windows_per_grid_point,
                                        custom_window_expansion_rate, range_query_time_interval_multiplier,
                                        knn_query_time_interval_multiplier, knn_k,
                                        use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log;

        log << "TRACE-Q Runtime vs Query Accuracy - Window expansion rate = 1.3\n";
        log << "Parameters:\n";
        log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log << "Max Threads: " << std::to_string(max_threads) << "\n";
        log << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log << "Window Expansion Rate: " << std::to_string(custom_window_expansion_rate) << "\n";
        log << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log << "KNN K: " << std::to_string(knn_k) << "\n";
        log << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log << "Benchmark:\n";
        log << "Runtime: " << time / 1000 << " s\n";
        log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
        log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
        log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();
    }

    SUBCASE("TRACE-Q Runtime vs Query Accuracy - Window expansion rate = 1.5") {

        double custom_window_expansion_rate = 1.5;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                        max_trajectories_in_batch, max_threads,
                                        range_query_grid_density,
                                        knn_query_grid_density, windows_per_grid_point,
                                        custom_window_expansion_rate, range_query_time_interval_multiplier,
                                        knn_query_time_interval_multiplier, knn_k,
                                        use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log;

        log << "TRACE-Q Runtime vs Query Accuracy - Window expansion rate = 1.5\n";
        log << "Parameters:\n";
        log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log << "Max Threads: " << std::to_string(max_threads) << "\n";
        log << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log << "Window Expansion Rate: " << std::to_string(custom_window_expansion_rate) << "\n";
        log << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log << "KNN K: " << std::to_string(knn_k) << "\n";
        log << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log << "Benchmark:\n";
        log << "Runtime: " << time / 1000 << " s\n";
        log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
        log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
        log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();
    }
}


TEST_CASE("TRACE_Q Range Query density and time interval benchmarking") {

    trajectory_data_handling::Trajectory_Manager::reset_all_data();
    trajectory_data_handling::File_Manager::load_tdrive_dataset();

    double resolution_scale = 1.1;
    double min_range_query_accuracy = 0.95;
    double min_knn_query_accuracy = 0.95;
    int max_trajectories_in_batch = 8;
    int max_threads = 50;
    //auto range_query_grid_density = 0.1;
    auto knn_query_grid_density = 0.1;
    int windows_per_grid_point = 4;
    double window_expansion_rate = 1.3;
    //double range_query_time_interval_multiplier = 0.1;
    double knn_query_time_interval_multiplier = 0.1;
    int knn_k = 10;
    bool use_KNN_for_query_accuracy = true;

    SUBCASE("TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.1") {

        double custom_range_query_grid_density = 0.1;
        double custom_range_query_time_interval_multiplier = 0.1;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                        max_trajectories_in_batch, max_threads,
                                        custom_range_query_grid_density,
                                        knn_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, custom_range_query_time_interval_multiplier,
                                        knn_query_time_interval_multiplier, knn_k,
                                        use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log;

        log << "TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.1\n";
        log << "Parameters:\n";
        log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log << "Max Threads: " << std::to_string(max_threads) << "\n";
        log << "Range Query Grid Density: " << std::to_string(custom_range_query_grid_density) << "\n";
        log << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log << "Range Query Time Interval Multiplier: " << std::to_string(custom_range_query_time_interval_multiplier) << "\n";
        log << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log << "KNN K: " << std::to_string(knn_k) << "\n";
        log << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log << "Benchmark:\n";
        log << "Runtime: " << time / 1000 << " s\n";
        log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
        log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
        log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();
    }

    SUBCASE("TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.05") {

        double custom_range_query_grid_density = 0.05;
        double custom_range_query_time_interval_multiplier = 0.05;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                        max_trajectories_in_batch, max_threads,
                                        custom_range_query_grid_density,
                                        knn_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, custom_range_query_time_interval_multiplier,
                                        knn_query_time_interval_multiplier, knn_k,
                                        use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log;

        log << "TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.05\n";
        log << "Parameters:\n";
        log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log << "Max Threads: " << std::to_string(max_threads) << "\n";
        log << "Range Query Grid Density: " << std::to_string(custom_range_query_grid_density) << "\n";
        log << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log << "Range Query Time Interval Multiplier: " << std::to_string(custom_range_query_time_interval_multiplier) << "\n";
        log << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log << "KNN K: " << std::to_string(knn_k) << "\n";
        log << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log << "Benchmark:\n";
        log << "Runtime: " << time / 1000 << " s\n";
        log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
        log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
        log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();
    }

    SUBCASE("TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.02") {

        double custom_range_query_grid_density = 0.02;
        double custom_range_query_time_interval_multiplier = 0.02;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                        max_trajectories_in_batch, max_threads,
                                        custom_range_query_grid_density,
                                        knn_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, custom_range_query_time_interval_multiplier,
                                        knn_query_time_interval_multiplier, knn_k,
                                        use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log;

        log << "TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.02\n";
        log << "Parameters:\n";
        log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log << "Max Threads: " << std::to_string(max_threads) << "\n";
        log << "Range Query Grid Density: " << std::to_string(custom_range_query_grid_density) << "\n";
        log << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log << "Range Query Time Interval Multiplier: " << std::to_string(custom_range_query_time_interval_multiplier) << "\n";
        log << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log << "KNN K: " << std::to_string(knn_k) << "\n";
        log << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log << "Benchmark:\n";
        log << "Runtime: " << time / 1000 << " s\n";
        log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
        log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
        log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();
    }

    SUBCASE("TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.2") {

        double custom_range_query_grid_density = 0.2;
        double custom_range_query_time_interval_multiplier = 0.2;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                        max_trajectories_in_batch, max_threads,
                                        custom_range_query_grid_density,
                                        knn_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, custom_range_query_time_interval_multiplier,
                                        knn_query_time_interval_multiplier, knn_k,
                                        use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log;

        log << "TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.2\n";
        log << "Parameters:\n";
        log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log << "Max Threads: " << std::to_string(max_threads) << "\n";
        log << "Range Query Grid Density: " << std::to_string(custom_range_query_grid_density) << "\n";
        log << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log << "Range Query Time Interval Multiplier: " << std::to_string(custom_range_query_time_interval_multiplier) << "\n";
        log << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log << "KNN K: " << std::to_string(knn_k) << "\n";
        log << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log << "Benchmark:\n";
        log << "Runtime: " << time / 1000 << " s\n";
        log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
        log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
        log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();
    }

    SUBCASE("TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.25") {

        double custom_range_query_grid_density = 0.25;
        double custom_range_query_time_interval_multiplier = 0.25;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                        max_trajectories_in_batch, max_threads,
                                        custom_range_query_grid_density,
                                        knn_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, custom_range_query_time_interval_multiplier,
                                        knn_query_time_interval_multiplier, knn_k,
                                        use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log;

        log << "TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.25\n";
        log << "Parameters:\n";
        log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log << "Max Threads: " << std::to_string(max_threads) << "\n";
        log << "Range Query Grid Density: " << std::to_string(custom_range_query_grid_density) << "\n";
        log << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log << "Range Query Time Interval Multiplier: " << std::to_string(custom_range_query_time_interval_multiplier) << "\n";
        log << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log << "KNN K: " << std::to_string(knn_k) << "\n";
        log << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log << "Benchmark:\n";
        log << "Runtime: " << time / 1000 << " s\n";
        log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
        log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
        log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();
    }
}

TEST_CASE("TRACE_Q KNN Query density and time interval benchmarking") {

    trajectory_data_handling::Trajectory_Manager::reset_all_data();
    trajectory_data_handling::File_Manager::load_tdrive_dataset();

    double resolution_scale = 1.1;
    double min_range_query_accuracy = 0.95;
    double min_knn_query_accuracy = 0.95;
    int max_trajectories_in_batch = 8;
    int max_threads = 50;
    auto range_query_grid_density = 0.1;
    //auto knn_query_grid_density = 0.1;
    int windows_per_grid_point = 4;
    double window_expansion_rate = 1.3;
    double range_query_time_interval_multiplier = 0.1;
    //double knn_query_time_interval_multiplier = 0.1;
    int knn_k = 10;
    bool use_KNN_for_query_accuracy = true;

    SUBCASE("TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.1") {

        double custom_knn_query_grid_density = 0.1;
        double custom_knn_query_time_interval_multiplier = 0.1;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                        max_trajectories_in_batch, max_threads,
                                        range_query_grid_density,
                                        custom_knn_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, range_query_time_interval_multiplier,
                                        custom_knn_query_time_interval_multiplier, knn_k,
                                        use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log;

        log << "TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.1\n";
        log << "Parameters:\n";
        log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log << "Max Threads: " << std::to_string(max_threads) << "\n";
        log << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log << "KNN Query Grid Density: " << std::to_string(custom_knn_query_grid_density) << "\n";
        log << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log << "Range KNN Time Interval Multiplier: " << std::to_string(custom_knn_query_time_interval_multiplier) << "\n";
        log << "KNN K: " << std::to_string(knn_k) << "\n";
        log << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log << "Benchmark:\n";
        log << "Runtime: " << time / 1000 << " s\n";
        log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
        log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
        log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();
    }

    SUBCASE("TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.05") {

        double custom_knn_query_grid_density = 0.05;
        double custom_knn_query_time_interval_multiplier = 0.05;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                        max_trajectories_in_batch, max_threads,
                                        range_query_grid_density,
                                        custom_knn_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, range_query_time_interval_multiplier,
                                        custom_knn_query_time_interval_multiplier, knn_k,
                                        use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log;

        log << "TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.05\n";
        log << "Parameters:\n";
        log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log << "Max Threads: " << std::to_string(max_threads) << "\n";
        log << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log << "KNN Query Grid Density: " << std::to_string(custom_knn_query_grid_density) << "\n";
        log << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log << "Range KNN Time Interval Multiplier: " << std::to_string(custom_knn_query_time_interval_multiplier) << "\n";
        log << "KNN K: " << std::to_string(knn_k) << "\n";
        log << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log << "Benchmark:\n";
        log << "Runtime: " << time / 1000 << " s\n";
        log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
        log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
        log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();
    }

    SUBCASE("TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.02") {

        double custom_knn_query_grid_density = 0.02;
        double custom_knn_query_time_interval_multiplier = 0.02;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                        max_trajectories_in_batch, max_threads,
                                        range_query_grid_density,
                                        custom_knn_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, range_query_time_interval_multiplier,
                                        custom_knn_query_time_interval_multiplier, knn_k,
                                        use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log;

        log << "TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.02\n";
        log << "Parameters:\n";
        log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log << "Max Threads: " << std::to_string(max_threads) << "\n";
        log << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log << "KNN Query Grid Density: " << std::to_string(custom_knn_query_grid_density) << "\n";
        log << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log << "Range KNN Time Interval Multiplier: " << std::to_string(custom_knn_query_time_interval_multiplier) << "\n";
        log << "KNN K: " << std::to_string(knn_k) << "\n";
        log << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log << "Benchmark:\n";
        log << "Runtime: " << time / 1000 << " s\n";
        log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
        log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
        log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();
    }

    SUBCASE("TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.2") {

        double custom_knn_query_grid_density = 0.2;
        double custom_knn_query_time_interval_multiplier = 0.2;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                        max_trajectories_in_batch, max_threads,
                                        range_query_grid_density,
                                        custom_knn_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, range_query_time_interval_multiplier,
                                        custom_knn_query_time_interval_multiplier, knn_k,
                                        use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log;

        log << "TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.2\n";
        log << "Parameters:\n";
        log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log << "Max Threads: " << std::to_string(max_threads) << "\n";
        log << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log << "KNN Query Grid Density: " << std::to_string(custom_knn_query_grid_density) << "\n";
        log << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log << "Range KNN Time Interval Multiplier: " << std::to_string(custom_knn_query_time_interval_multiplier) << "\n";
        log << "KNN K: " << std::to_string(knn_k) << "\n";
        log << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log << "Benchmark:\n";
        log << "Runtime: " << time / 1000 << " s\n";
        log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
        log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
        log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();
    }

    SUBCASE("TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.25") {

        double custom_knn_query_grid_density = 0.25;
        double custom_knn_query_time_interval_multiplier = 0.25;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                        max_trajectories_in_batch, max_threads,
                                        range_query_grid_density,
                                        custom_knn_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, range_query_time_interval_multiplier,
                                        custom_knn_query_time_interval_multiplier, knn_k,
                                        use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log;

        log << "TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.25\n";
        log << "Parameters:\n";
        log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log << "Max Threads: " << std::to_string(max_threads) << "\n";
        log << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log << "KNN Query Grid Density: " << std::to_string(custom_knn_query_grid_density) << "\n";
        log << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log << "Range KNN Time Interval Multiplier: " << std::to_string(custom_knn_query_time_interval_multiplier) << "\n";
        log << "KNN K: " << std::to_string(knn_k) << "\n";
        log << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log << "Benchmark:\n";
        log << "Runtime: " << time / 1000 << " s\n";
        log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
        log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
        log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();
    }

}


TEST_CASE("TRACE-Q KNN K benchmarking") {

    /*
     * The TRACE-Q benchmark tests assume that the dataset consist of 50 trajectories.
     */

    trajectory_data_handling::Trajectory_Manager::reset_all_data();
    trajectory_data_handling::File_Manager::load_tdrive_dataset();

    double resolution_scale = 1.1;
    double min_range_query_accuracy = 0.95;
    double min_knn_query_accuracy = 0.95;
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

        int custom_knn_k = 1;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                        max_trajectories_in_batch, max_threads,
                                        range_query_grid_density,
                                        knn_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, range_query_time_interval_multiplier,
                                        knn_query_time_interval_multiplier, custom_knn_k,
                                        use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log;

        log << "TRACE-Q Runtime vs Query Accuracy - KNN K = 1\n";
        log << "Parameters:\n";
        log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log << "Max Threads: " << std::to_string(max_threads) << "\n";
        log << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log << "KNN K: " << std::to_string(custom_knn_k) << "\n";
        log << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log << "Benchmark:\n";
        log << "Runtime: " << time / 1000 << " s\n";
        log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
        log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
        log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();
    }

    SUBCASE("TRACE-Q Runtime vs Query Accuracy - KNN K = 10") {
        int custom_knn_k = 10;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                        max_trajectories_in_batch, max_threads,
                                        range_query_grid_density,
                                        knn_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, range_query_time_interval_multiplier,
                                        knn_query_time_interval_multiplier, custom_knn_k,
                                        use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log;

        log << "TRACE-Q Runtime vs Query Accuracy - KNN K = 10\n";
        log << "Parameters:\n";
        log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log << "Max Threads: " << std::to_string(max_threads) << "\n";
        log << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log << "KNN K: " << std::to_string(custom_knn_k) << "\n";
        log << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log << "Benchmark:\n";
        log << "Runtime: " << time / 1000 << " s\n";
        log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
        log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
        log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();
    }

    SUBCASE("TRACE-Q Runtime vs Query Accuracy - KNN K = 50") {
        int custom_knn_k = 50;

        auto trace_q = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                        max_trajectories_in_batch, max_threads,
                                        range_query_grid_density,
                                        knn_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, range_query_time_interval_multiplier,
                                        knn_query_time_interval_multiplier, custom_knn_k,
                                        use_KNN_for_query_accuracy};
        auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log;

        log << "TRACE-Q Runtime vs Query Accuracy - KNN K = 50\n";
        log << "Parameters:\n";
        log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log << "Max Threads: " << std::to_string(max_threads) << "\n";
        log << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log << "KNN K: " << std::to_string(custom_knn_k) << "\n";
        log << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log << "Benchmark:\n";
        log << "Runtime: " << time / 1000 << " s\n";
        log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
        log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
        log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset();
    }
}

TEST_CASE("TRACE-Q VS RL4QDTS") {

    auto evil_query_objects = analytics::Benchmark::evil_initialize_query_objects(); // This is where the magic happens

    double resolution_scale = 1.1;
    double min_range_query_accuracy = 0.95;
    double min_knn_query_accuracy = 0.95;
    int max_trajectories_in_batch = 8;
    int max_threads = 50;
    auto range_query_grid_density = 0.1;
    auto knn_query_grid_density = 0.1;
    int windows_per_grid_point = 3;
    double window_expansion_rate = 1.3;
    double range_query_time_interval_multiplier = 0.1;
    double knn_query_time_interval_multiplier = 0.1;
    int knn_k = 10;
    bool use_KNN_for_query_accuracy = true;

    auto trace_q = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                    max_trajectories_in_batch, max_threads,
                                    range_query_grid_density,
                                    knn_query_grid_density, windows_per_grid_point,
                                    window_expansion_rate, range_query_time_interval_multiplier,
                                    knn_query_time_interval_multiplier, knn_k,
                                    use_KNN_for_query_accuracy};

    auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

    auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(evil_query_objects);

    std::stringstream log;

    log << "TRACE-Q VS RL4QDTS\n";
    log << "Parameters:\n";
    log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
    log << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
    log << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
    log << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
    log << "Max Threads: " << std::to_string(max_threads) << "\n";
    log << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
    log << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
    log << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
    log << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
    log << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
    log << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
    log << "KNN K: " << std::to_string(knn_k) << "\n";
    log << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
    log << "Benchmark:\n";
    log << "Runtime: " << time / 1000 << " s\n";
    log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
    log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
    log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
    logger << log.str();

    // Teardown
    trajectory_data_handling::Trajectory_Manager::reset_all_data();
    trajectory_data_handling::File_Manager::load_tdrive_dataset();

}

TEST_CASE("TRACE-Q Hardcore Query Accuracy") {

    trajectory_data_handling::Trajectory_Manager::reset_all_data();
    trajectory_data_handling::File_Manager::load_tdrive_dataset();

    double resolution_scale = 1.1;
    double min_range_query_accuracy = 0.95;
    double min_knn_query_accuracy = 0.95;
    int max_trajectories_in_batch = 8;
    int max_threads = 50;
    auto range_query_grid_density = 0.1;
    auto knn_query_grid_density = 0.1;
    int windows_per_grid_point = 3;
    double window_expansion_rate = 1.3;
    double range_query_time_interval_multiplier = 0.1;
    double knn_query_time_interval_multiplier = 0.1;
    int knn_k = 10;
    bool use_KNN_for_query_accuracy = true;

    auto trace_q = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                    max_trajectories_in_batch, max_threads,
                                    range_query_grid_density,
                                    knn_query_grid_density, windows_per_grid_point,
                                    window_expansion_rate, range_query_time_interval_multiplier,
                                    knn_query_time_interval_multiplier, knn_k,
                                    use_KNN_for_query_accuracy};

    auto time = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

    auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(query_objects);

    std::stringstream log;

    log << "TRACE-Q Hardcore mode (Enemies deal more damage, your hunger depletes faster, query accuracy is very unforgiving, and your save file is deleted if you die.)\n";
    log << "Parameters:\n";
    log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
    log << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
    log << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
    log << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
    log << "Max Threads: " << std::to_string(max_threads) << "\n";
    log << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
    log << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
    log << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
    log << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
    log << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
    log << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
    log << "KNN K: " << std::to_string(knn_k) << "\n";
    log << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
    log << "Benchmark:\n";
    log << "Runtime: " << time / 1000 << " s\n";
    log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
    log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
    log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
    logger << log.str();

    // Teardown
    trajectory_data_handling::Trajectory_Manager::reset_all_data();
    trajectory_data_handling::File_Manager::load_tdrive_dataset();
}
