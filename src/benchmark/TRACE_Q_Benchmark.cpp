#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "TRACE_Q_Benchmark.hpp"
#include "../simp-algorithms/TRACE_Q.hpp"
#include "../trajectory_data_handling/Trajectory_Manager.hpp"
#include "../trajectory_data_handling/File_Manager.hpp"
#include "Benchmark.hpp"

namespace analytics {

    void TRACE_Q_Benchmark::run_traceq_benchmarks(int amount_of_test_trajectories) {

        /*
         * The TRACE-Q benchmark tests assume that the dataset consist of 50 trajectories.
         */

        auto file_logger = analytics::Benchmark::get_logger();
        trajectory_data_handling::Trajectory_Manager::reset_all_data();
        trajectory_data_handling::File_Manager::load_tdrive_dataset(amount_of_test_trajectories);

        std::cout << "TRACE-Q Benchmark" << std::endl;

        const auto query_objects = analytics::Benchmark::initialize_query_objects();

        TRACE_Q_Benchmark::traceq_is_knn_necessary(query_objects, file_logger);
        TRACE_Q_Benchmark::traceq_window_expansion_rate(query_objects, file_logger);
        TRACE_Q_Benchmark::traceq_range_query_density_and_time_interval(query_objects, file_logger);
        TRACE_Q_Benchmark::traceq_knn_query_density_and_time_interval(query_objects, file_logger);
        TRACE_Q_Benchmark::traceq_knn_k(query_objects, file_logger);
    }

    void TRACE_Q_Benchmark::run_traceq_vs_mrpa(int amount_of_test_trajectory) {
        auto file_logger = analytics::Benchmark::get_logger();

        std::cout << "TRACE-Q vs MRPA Benchmarks" << std::endl;

        TRACE_Q_Benchmark::traceq_hardcore_query_accuracy(amount_of_test_trajectory, file_logger);
        TRACE_Q_Benchmark::mrpa_benchmark(amount_of_test_trajectory, file_logger);
    }

    void TRACE_Q_Benchmark::traceq_is_knn_necessary(std::vector<std::shared_ptr<Benchmark_Query>> const& query_objects,
                                                    logging::Logger & logger) {

        std::cout << "TEST CASE: TRACE-Q IS KNN NECESSARY?" << std::endl;

        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();

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


        // SUBCASE: TRACE-Q WITH KNN

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
        auto time1 = analytics::Benchmark::function_time([&trace_q]() { trace_q.run(); });

        auto query_accuracy1 = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log1;

        log1 << "TRACE-Q WITH KNN\n";
        log1 << "Parameters:\n";
        log1 << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log1 << "Min Range Query Accuracy: " << std::to_string(custom_min_range_query_accuracy) << "\n";
        log1 << "Min KNN Query Accuracy: " << std::to_string(custom_min_knn_query_accuracy) << "\n";
        log1 << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log1 << "Max Threads: " << std::to_string(max_threads) << "\n";
        log1 << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log1 << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log1 << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log1 << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log1 << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log1 << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log1 << "KNN K: " << std::to_string(knn_k) << "\n";
        log1 << "Use KNN For Query Accuracy: " << std::to_string(custom_use_KNN_for_query_accuracy) << "\n";
        log1 << "Benchmark:\n";
        log1 << "Runtime: " << time1 / 1000 << " s\n";
        log1 << "Range Query Accuracy: " << query_accuracy1.range_f1 << "\n";
        log1 << "KNN Query Accuracy: " << query_accuracy1.knn_f1 << "\n";
        log1 << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log1.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();


        // SUBCASE: TRACE-Q WITHOUT KNN

        custom_use_KNN_for_query_accuracy = false;

        auto trace_q_no_knn = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, max_trajectories_in_batch, max_threads,
                                        range_query_grid_density, windows_per_grid_point,
                                        window_expansion_rate, range_query_time_interval_multiplier,
                                        custom_use_KNN_for_query_accuracy};
        auto time2 = analytics::Benchmark::function_time([&trace_q_no_knn]() { trace_q_no_knn.run(); });

        auto query_accuracy2 = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log2;

        log2 << "TRACE-Q WITHOUT KNN\n";
        log2 << "Parameters:\n";
        log2 << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log2 << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log2 << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log2 << "Max Threads: " << std::to_string(max_threads) << "\n";
        log2 << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log2 << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log2 << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log2 << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log2 << "Use KNN For Query Accuracy: " << std::to_string(custom_use_KNN_for_query_accuracy) << "\n";
        log2 << "Benchmark:\n";
        log2 << "Runtime: " << time2 / 1000 << " s\n";
        log2 << "Range Query Accuracy: " << query_accuracy2.range_f1 << "\n";
        log2 << "KNN Query Accuracy: " << query_accuracy2.knn_f1 << "\n";
        log2 << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << std::endl;
        logger << log2.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();
    }

    void TRACE_Q_Benchmark::traceq_window_expansion_rate(std::vector<std::shared_ptr<Benchmark_Query>> const& query_objects,
                                      logging::Logger & logger) {

        std::cout << "TEST CASE: TRACE_Q Window expansion rate benchmarking" << std::endl;
        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();

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

        // SUBCASE: TRACE-Q Runtime vs Query Accuracy - Window expansion rate = 1.2

        double custom_window_expansion_rate = 1.2;

        auto trace_q1 = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                         max_trajectories_in_batch, max_threads,
                                         range_query_grid_density,
                                         knn_query_grid_density, windows_per_grid_point,
                                         custom_window_expansion_rate, range_query_time_interval_multiplier,
                                         knn_query_time_interval_multiplier, knn_k,
                                         use_KNN_for_query_accuracy};
        auto time1 = analytics::Benchmark::function_time([&trace_q1]() { trace_q1.run(); });

        auto query_accuracy1 = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log1;

        log1 << "TRACE-Q Runtime vs Query Accuracy - Window expansion rate = 1.2\n";
        log1 << "Parameters:\n";
        log1 << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log1 << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log1 << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log1 << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log1 << "Max Threads: " << std::to_string(max_threads) << "\n";
        log1 << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log1 << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log1 << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log1 << "Window Expansion Rate: " << std::to_string(custom_window_expansion_rate) << "\n";
        log1 << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log1 << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log1 << "KNN K: " << std::to_string(knn_k) << "\n";
        log1 << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log1 << "Benchmark:\n";
        log1 << "Runtime: " << time1 / 1000 << " s\n";
        log1 << "Range Query Accuracy: " << query_accuracy1.range_f1 << "\n";
        log1 << "KNN Query Accuracy: " << query_accuracy1.knn_f1 << "\n";
        log1 << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log1.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();


        // SUBCASE: TRACE-Q Runtime vs Query Accuracy - Window expansion rate = 1.3

        custom_window_expansion_rate = 1.3;

        auto trace_q2 = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                         max_trajectories_in_batch, max_threads,
                                         range_query_grid_density,
                                         knn_query_grid_density, windows_per_grid_point,
                                         custom_window_expansion_rate, range_query_time_interval_multiplier,
                                         knn_query_time_interval_multiplier, knn_k,
                                         use_KNN_for_query_accuracy};
        auto time2 = analytics::Benchmark::function_time([&trace_q2]() { trace_q2.run(); });

        auto query_accuracy2 = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log2;

        log2 << "TRACE-Q Runtime vs Query Accuracy - Window expansion rate = 1.3\n";
        log2 << "Parameters:\n";
        log2 << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log2 << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log2 << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log2 << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log2 << "Max Threads: " << std::to_string(max_threads) << "\n";
        log2 << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log2 << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log2 << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log2 << "Window Expansion Rate: " << std::to_string(custom_window_expansion_rate) << "\n";
        log2 << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log2 << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log2 << "KNN K: " << std::to_string(knn_k) << "\n";
        log2 << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log2 << "Benchmark:\n";
        log2 << "Runtime: " << time2 / 1000 << " s\n";
        log2 << "Range Query Accuracy: " << query_accuracy2.range_f1 << "\n";
        log2 << "KNN Query Accuracy: " << query_accuracy2.knn_f1 << "\n";
        log2 << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log2.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();


        // SUBCASE: TRACE-Q Runtime vs Query Accuracy - Window expansion rate = 1.5

        custom_window_expansion_rate = 1.5;

        auto trace_q3 = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                         max_trajectories_in_batch, max_threads,
                                         range_query_grid_density,
                                         knn_query_grid_density, windows_per_grid_point,
                                         custom_window_expansion_rate, range_query_time_interval_multiplier,
                                         knn_query_time_interval_multiplier, knn_k,
                                         use_KNN_for_query_accuracy};
        auto time3 = analytics::Benchmark::function_time([&trace_q3]() { trace_q3.run(); });

        auto query_accuracy3 = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log3;

        log3 << "TRACE-Q Runtime vs Query Accuracy - Window expansion rate = 1.5\n";
        log3 << "Parameters:\n";
        log3 << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log3 << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log3 << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log3 << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log3 << "Max Threads: " << std::to_string(max_threads) << "\n";
        log3 << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log3 << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log3 << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log3 << "Window Expansion Rate: " << std::to_string(custom_window_expansion_rate) << "\n";
        log3 << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log3 << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log3 << "KNN K: " << std::to_string(knn_k) << "\n";
        log3 << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log3 << "Benchmark:\n";
        log3 << "Runtime: " << time3 / 1000 << " s\n";
        log3 << "Range Query Accuracy: " << query_accuracy3.range_f1 << "\n";
        log3 << "KNN Query Accuracy: " << query_accuracy3.knn_f1 << "\n";
        log3 << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log3.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();

    }

    void TRACE_Q_Benchmark::traceq_range_query_density_and_time_interval(std::vector<std::shared_ptr<Benchmark_Query>> const& query_objects,
                                                                         logging::Logger & logger) {

        std::cout << "TRACE_Q Range Query density and time interval benchmarking" << std::endl;

        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();

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

        // SUBCASE: TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.1

        double custom_range_query_grid_density = 0.1;
        double custom_range_query_time_interval_multiplier = 0.1;

        auto trace_q1 = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                         max_trajectories_in_batch, max_threads,
                                         custom_range_query_grid_density,
                                         knn_query_grid_density, windows_per_grid_point,
                                         window_expansion_rate, custom_range_query_time_interval_multiplier,
                                         knn_query_time_interval_multiplier, knn_k,
                                         use_KNN_for_query_accuracy};
        auto time1 = analytics::Benchmark::function_time([&trace_q1]() { trace_q1.run(); });

        auto query_accuracy1 = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log1;

        log1 << "TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.1\n";
        log1 << "Parameters:\n";
        log1 << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log1 << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log1 << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log1 << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log1 << "Max Threads: " << std::to_string(max_threads) << "\n";
        log1 << "Range Query Grid Density: " << std::to_string(custom_range_query_grid_density) << "\n";
        log1 << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log1 << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log1 << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log1 << "Range Query Time Interval Multiplier: " << std::to_string(custom_range_query_time_interval_multiplier) << "\n";
        log1 << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log1 << "KNN K: " << std::to_string(knn_k) << "\n";
        log1 << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log1 << "Benchmark:\n";
        log1 << "Runtime: " << time1 / 1000 << " s\n";
        log1 << "Range Query Accuracy: " << query_accuracy1.range_f1 << "\n";
        log1 << "KNN Query Accuracy: " << query_accuracy1.knn_f1 << "\n";
        log1 << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log1.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();


        // SUBCASE: TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.05

        custom_range_query_grid_density = 0.05;
        custom_range_query_time_interval_multiplier = 0.05;

        auto trace_q2 = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                         max_trajectories_in_batch, max_threads,
                                         custom_range_query_grid_density,
                                         knn_query_grid_density, windows_per_grid_point,
                                         window_expansion_rate, custom_range_query_time_interval_multiplier,
                                         knn_query_time_interval_multiplier, knn_k,
                                         use_KNN_for_query_accuracy};
        auto time2 = analytics::Benchmark::function_time([&trace_q2]() { trace_q2.run(); });

        auto query_accuracy2 = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log2;

        log2 << "TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.05\n";
        log2 << "Parameters:\n";
        log2 << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log2 << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log2 << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log2 << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log2 << "Max Threads: " << std::to_string(max_threads) << "\n";
        log2 << "Range Query Grid Density: " << std::to_string(custom_range_query_grid_density) << "\n";
        log2 << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log2 << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log2 << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log2 << "Range Query Time Interval Multiplier: " << std::to_string(custom_range_query_time_interval_multiplier) << "\n";
        log2 << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log2 << "KNN K: " << std::to_string(knn_k) << "\n";
        log2 << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log2 << "Benchmark:\n";
        log2 << "Runtime: " << time2 / 1000 << " s\n";
        log2 << "Range Query Accuracy: " << query_accuracy2.range_f1 << "\n";
        log2 << "KNN Query Accuracy: " << query_accuracy2.knn_f1 << "\n";
        log2 << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log2.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();


        // SUBCASE: TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.02

        custom_range_query_grid_density = 0.02;
        custom_range_query_time_interval_multiplier = 0.02;

        auto trace_q3 = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                         max_trajectories_in_batch, max_threads,
                                         custom_range_query_grid_density,
                                         knn_query_grid_density, windows_per_grid_point,
                                         window_expansion_rate, custom_range_query_time_interval_multiplier,
                                         knn_query_time_interval_multiplier, knn_k,
                                         use_KNN_for_query_accuracy};
        auto time3 = analytics::Benchmark::function_time([&trace_q3]() { trace_q3.run(); });

        auto query_accuracy3 = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log3;

        log3 << "TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.02\n";
        log3 << "Parameters:\n";
        log3 << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log3 << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log3 << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log3 << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log3 << "Max Threads: " << std::to_string(max_threads) << "\n";
        log3 << "Range Query Grid Density: " << std::to_string(custom_range_query_grid_density) << "\n";
        log3 << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log3 << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log3 << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log3 << "Range Query Time Interval Multiplier: " << std::to_string(custom_range_query_time_interval_multiplier) << "\n";
        log3 << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log3 << "KNN K: " << std::to_string(knn_k) << "\n";
        log3 << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log3 << "Benchmark:\n";
        log3 << "Runtime: " << time3 / 1000 << " s\n";
        log3 << "Range Query Accuracy: " << query_accuracy3.range_f1 << "\n";
        log3 << "KNN Query Accuracy: " << query_accuracy3.knn_f1 << "\n";
        log3 << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log3.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();

        // SUBCASE: TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.2

        custom_range_query_grid_density = 0.2;
        custom_range_query_time_interval_multiplier = 0.2;

        auto trace_q4 = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                         max_trajectories_in_batch, max_threads,
                                         custom_range_query_grid_density,
                                         knn_query_grid_density, windows_per_grid_point,
                                         window_expansion_rate, custom_range_query_time_interval_multiplier,
                                         knn_query_time_interval_multiplier, knn_k,
                                         use_KNN_for_query_accuracy};
        auto time4 = analytics::Benchmark::function_time([&trace_q4]() { trace_q4.run(); });

        auto query_accuracy4 = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log4;

        log4 << "TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.2\n";
        log4 << "Parameters:\n";
        log4 << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log4 << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log4 << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log4 << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log4 << "Max Threads: " << std::to_string(max_threads) << "\n";
        log4 << "Range Query Grid Density: " << std::to_string(custom_range_query_grid_density) << "\n";
        log4 << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log4 << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log4 << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log4 << "Range Query Time Interval Multiplier: " << std::to_string(custom_range_query_time_interval_multiplier) << "\n";
        log4 << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log4 << "KNN K: " << std::to_string(knn_k) << "\n";
        log4 << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log4 << "Benchmark:\n";
        log4 << "Runtime: " << time4 / 1000 << " s\n";
        log4 << "Range Query Accuracy: " << query_accuracy4.range_f1 << "\n";
        log4 << "KNN Query Accuracy: " << query_accuracy4.knn_f1 << "\n";
        log4 << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log4.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();


        // SUBCASE: TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.25

        custom_range_query_grid_density = 0.25;
        custom_range_query_time_interval_multiplier = 0.25;

        auto trace_q5 = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                         max_trajectories_in_batch, max_threads,
                                         custom_range_query_grid_density,
                                         knn_query_grid_density, windows_per_grid_point,
                                         window_expansion_rate, custom_range_query_time_interval_multiplier,
                                         knn_query_time_interval_multiplier, knn_k,
                                         use_KNN_for_query_accuracy};
        auto time5 = analytics::Benchmark::function_time([&trace_q5]() { trace_q5.run(); });

        auto query_accuracy5 = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log5;

        log5 << "TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.25\n";
        log5 << "Parameters:\n";
        log5 << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log5 << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log5 << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log5 << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log5 << "Max Threads: " << std::to_string(max_threads) << "\n";
        log5 << "Range Query Grid Density: " << std::to_string(custom_range_query_grid_density) << "\n";
        log5 << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log5 << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log5 << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log5 << "Range Query Time Interval Multiplier: " << std::to_string(custom_range_query_time_interval_multiplier) << "\n";
        log5 << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log5 << "KNN K: " << std::to_string(knn_k) << "\n";
        log5 << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log5 << "Benchmark:\n";
        log5 << "Runtime: " << time5 / 1000 << " s\n";
        log5 << "Range Query Accuracy: " << query_accuracy5.range_f1 << "\n";
        log5 << "KNN Query Accuracy: " << query_accuracy5.knn_f1 << "\n";
        log5 << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log5.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();

    }

    void TRACE_Q_Benchmark::traceq_knn_query_density_and_time_interval(std::vector<std::shared_ptr<Benchmark_Query>> const& query_objects,
                                                    logging::Logger & logger) {

        std::cout << "TRACE_Q KNN Query density and time interval benchmarking" << std::endl;

        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();

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


        // SUBCASE: TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.1

        double custom_knn_query_grid_density = 0.1;
        double custom_knn_query_time_interval_multiplier = 0.1;

        auto trace_q1 = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                         max_trajectories_in_batch, max_threads,
                                         range_query_grid_density,
                                         custom_knn_query_grid_density, windows_per_grid_point,
                                         window_expansion_rate, range_query_time_interval_multiplier,
                                         custom_knn_query_time_interval_multiplier, knn_k,
                                         use_KNN_for_query_accuracy};
        auto time1 = analytics::Benchmark::function_time([&trace_q1]() { trace_q1.run(); });

        auto query_accuracy1 = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log1;

        log1 << "TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.1\n";
        log1 << "Parameters:\n";
        log1 << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log1 << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log1 << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log1 << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log1 << "Max Threads: " << std::to_string(max_threads) << "\n";
        log1 << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log1 << "KNN Query Grid Density: " << std::to_string(custom_knn_query_grid_density) << "\n";
        log1 << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log1 << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log1 << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log1 << "Range KNN Time Interval Multiplier: " << std::to_string(custom_knn_query_time_interval_multiplier) << "\n";
        log1 << "KNN K: " << std::to_string(knn_k) << "\n";
        log1 << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log1 << "Benchmark:\n";
        log1 << "Runtime: " << time1 / 1000 << " s\n";
        log1 << "Range Query Accuracy: " << query_accuracy1.range_f1 << "\n";
        log1 << "KNN Query Accuracy: " << query_accuracy1.knn_f1 << "\n";
        log1 << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log1.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();


        // SUBCASE: TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.05

        custom_knn_query_grid_density = 0.05;
        custom_knn_query_time_interval_multiplier = 0.05;

        auto trace_q2 = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                         max_trajectories_in_batch, max_threads,
                                         range_query_grid_density,
                                         custom_knn_query_grid_density, windows_per_grid_point,
                                         window_expansion_rate, range_query_time_interval_multiplier,
                                         custom_knn_query_time_interval_multiplier, knn_k,
                                         use_KNN_for_query_accuracy};
        auto time2 = analytics::Benchmark::function_time([&trace_q2]() { trace_q2.run(); });

        auto query_accuracy2 = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log2;

        log2 << "TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.05\n";
        log2 << "Parameters:\n";
        log2 << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log2 << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log2 << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log2 << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log2 << "Max Threads: " << std::to_string(max_threads) << "\n";
        log2 << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log2 << "KNN Query Grid Density: " << std::to_string(custom_knn_query_grid_density) << "\n";
        log2 << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log2 << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log2 << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log2 << "Range KNN Time Interval Multiplier: " << std::to_string(custom_knn_query_time_interval_multiplier) << "\n";
        log2 << "KNN K: " << std::to_string(knn_k) << "\n";
        log2 << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log2 << "Benchmark:\n";
        log2 << "Runtime: " << time2 / 1000 << " s\n";
        log2 << "Range Query Accuracy: " << query_accuracy2.range_f1 << "\n";
        log2 << "KNN Query Accuracy: " << query_accuracy2.knn_f1 << "\n";
        log2 << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log2.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();


        // SUBCASE: TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.02

        custom_knn_query_grid_density = 0.02;
        custom_knn_query_time_interval_multiplier = 0.02;

        auto trace_q3 = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                         max_trajectories_in_batch, max_threads,
                                         range_query_grid_density,
                                         custom_knn_query_grid_density, windows_per_grid_point,
                                         window_expansion_rate, range_query_time_interval_multiplier,
                                         custom_knn_query_time_interval_multiplier, knn_k,
                                         use_KNN_for_query_accuracy};
        auto time3 = analytics::Benchmark::function_time([&trace_q3]() { trace_q3.run(); });

        auto query_accuracy3 = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log3;

        log3 << "TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.02\n";
        log3 << "Parameters:\n";
        log3 << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log3 << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log3 << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log3 << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log3 << "Max Threads: " << std::to_string(max_threads) << "\n";
        log3 << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log3 << "KNN Query Grid Density: " << std::to_string(custom_knn_query_grid_density) << "\n";
        log3 << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log3 << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log3 << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log3 << "Range KNN Time Interval Multiplier: " << std::to_string(custom_knn_query_time_interval_multiplier) << "\n";
        log3 << "KNN K: " << std::to_string(knn_k) << "\n";
        log3 << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log3 << "Benchmark:\n";
        log3 << "Runtime: " << time3 / 1000 << " s\n";
        log3 << "Range Query Accuracy: " << query_accuracy3.range_f1 << "\n";
        log3 << "KNN Query Accuracy: " << query_accuracy3.knn_f1 << "\n";
        log3 << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log3.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();

        // SUBCASE: TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.2

        custom_knn_query_grid_density = 0.2;
        custom_knn_query_time_interval_multiplier = 0.2;

        auto trace_q4 = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                         max_trajectories_in_batch, max_threads,
                                         range_query_grid_density,
                                         custom_knn_query_grid_density, windows_per_grid_point,
                                         window_expansion_rate, range_query_time_interval_multiplier,
                                         custom_knn_query_time_interval_multiplier, knn_k,
                                         use_KNN_for_query_accuracy};
        auto time4 = analytics::Benchmark::function_time([&trace_q4]() { trace_q4.run(); });

        auto query_accuracy4 = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log4;

        log4 << "TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.2\n";
        log4 << "Parameters:\n";
        log4 << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log4 << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log4 << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log4 << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log4 << "Max Threads: " << std::to_string(max_threads) << "\n";
        log4 << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log4 << "KNN Query Grid Density: " << std::to_string(custom_knn_query_grid_density) << "\n";
        log4 << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log4 << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log4 << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log4 << "Range KNN Time Interval Multiplier: " << std::to_string(custom_knn_query_time_interval_multiplier) << "\n";
        log4 << "KNN K: " << std::to_string(knn_k) << "\n";
        log4 << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log4 << "Benchmark:\n";
        log4 << "Runtime: " << time4 / 1000 << " s\n";
        log4 << "Range Query Accuracy: " << query_accuracy4.range_f1 << "\n";
        log4 << "KNN Query Accuracy: " << query_accuracy4.knn_f1 << "\n";
        log4 << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log4.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();


        // SUBCASE: TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.25

        custom_knn_query_grid_density = 0.25;
        custom_knn_query_time_interval_multiplier = 0.25;

        auto trace_q5 = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                         max_trajectories_in_batch, max_threads,
                                         range_query_grid_density,
                                         custom_knn_query_grid_density, windows_per_grid_point,
                                         window_expansion_rate, range_query_time_interval_multiplier,
                                         custom_knn_query_time_interval_multiplier, knn_k,
                                         use_KNN_for_query_accuracy};
        auto time5 = analytics::Benchmark::function_time([&trace_q5]() { trace_q5.run(); });

        auto query_accuracy5 = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log5;

        log5 << "TRACE-Q Runtime vs Query Accuracy - Density and time interval = 0.25\n";
        log5 << "Parameters:\n";
        log5 << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log5 << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log5 << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log5 << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log5 << "Max Threads: " << std::to_string(max_threads) << "\n";
        log5 << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log5 << "KNN Query Grid Density: " << std::to_string(custom_knn_query_grid_density) << "\n";
        log5 << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log5 << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log5 << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log5 << "Range KNN Time Interval Multiplier: " << std::to_string(custom_knn_query_time_interval_multiplier) << "\n";
        log5 << "KNN K: " << std::to_string(knn_k) << "\n";
        log5 << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log5 << "Benchmark:\n";
        log5 << "Runtime: " << time5 / 1000 << " s\n";
        log5 << "Range Query Accuracy: " << query_accuracy5.range_f1 << "\n";
        log5 << "KNN Query Accuracy: " << query_accuracy5.knn_f1 << "\n";
        log5 << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log5.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();

    }

    void TRACE_Q_Benchmark::traceq_knn_k(std::vector<std::shared_ptr<Benchmark_Query>> const& query_objects,
                      logging::Logger & logger) {

        std::cout << "TRACE-Q KNN K benchmarking" << std::endl;

        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();

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


        // SUBCASE: TRACE-Q Runtime vs Query Accuracy - KNN K = 1

        int custom_knn_k = 1;

        auto trace_q1 = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                         max_trajectories_in_batch, max_threads,
                                         range_query_grid_density,
                                         knn_query_grid_density, windows_per_grid_point,
                                         window_expansion_rate, range_query_time_interval_multiplier,
                                         knn_query_time_interval_multiplier, custom_knn_k,
                                         use_KNN_for_query_accuracy};
        auto time1 = analytics::Benchmark::function_time([&trace_q1]() { trace_q1.run(); });

        auto query_accuracy1 = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log1;

        log1 << "TRACE-Q Runtime vs Query Accuracy - KNN K = 1\n";
        log1 << "Parameters:\n";
        log1 << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log1 << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log1 << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log1 << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log1 << "Max Threads: " << std::to_string(max_threads) << "\n";
        log1 << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log1 << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log1 << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log1 << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log1 << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log1 << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log1 << "KNN K: " << std::to_string(custom_knn_k) << "\n";
        log1 << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log1 << "Benchmark:\n";
        log1 << "Runtime: " << time1 / 1000 << " s\n";
        log1 << "Range Query Accuracy: " << query_accuracy1.range_f1 << "\n";
        log1 << "KNN Query Accuracy: " << query_accuracy1.knn_f1 << "\n";
        log1 << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log1.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();


        // SUBCASE: TRACE-Q Runtime vs Query Accuracy - KNN K = 10

        custom_knn_k = 10;

        auto trace_q2 = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                         max_trajectories_in_batch, max_threads,
                                         range_query_grid_density,
                                         knn_query_grid_density, windows_per_grid_point,
                                         window_expansion_rate, range_query_time_interval_multiplier,
                                         knn_query_time_interval_multiplier, custom_knn_k,
                                         use_KNN_for_query_accuracy};
        auto time2 = analytics::Benchmark::function_time([&trace_q2]() { trace_q2.run(); });

        auto query_accuracy2 = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log2;

        log2 << "TRACE-Q Runtime vs Query Accuracy - KNN K = 10\n";
        log2 << "Parameters:\n";
        log2 << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log2 << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log2 << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log2 << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log2 << "Max Threads: " << std::to_string(max_threads) << "\n";
        log2 << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log2 << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log2 << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log2 << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log2 << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log2 << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log2 << "KNN K: " << std::to_string(custom_knn_k) << "\n";
        log2 << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log2 << "Benchmark:\n";
        log2 << "Runtime: " << time2 / 1000 << " s\n";
        log2 << "Range Query Accuracy: " << query_accuracy2.range_f1 << "\n";
        log2 << "KNN Query Accuracy: " << query_accuracy2.knn_f1 << "\n";
        log2 << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log2.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();


        // SUBCASE: TRACE-Q Runtime vs Query Accuracy - KNN K = 50

        custom_knn_k = 50;

        auto trace_q3 = trace_q::TRACE_Q{resolution_scale, min_range_query_accuracy, min_knn_query_accuracy,
                                         max_trajectories_in_batch, max_threads,
                                         range_query_grid_density,
                                         knn_query_grid_density, windows_per_grid_point,
                                         window_expansion_rate, range_query_time_interval_multiplier,
                                         knn_query_time_interval_multiplier, custom_knn_k,
                                         use_KNN_for_query_accuracy};
        auto time3 = analytics::Benchmark::function_time([&trace_q3]() { trace_q3.run(); });

        auto query_accuracy3 = analytics::Benchmark::benchmark_query_accuracy(query_objects);

        std::stringstream log3;

        log3 << "TRACE-Q Runtime vs Query Accuracy - KNN K = 50\n";
        log3 << "Parameters:\n";
        log3 << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
        log3 << "Min Range Query Accuracy: " << std::to_string(min_range_query_accuracy) << "\n";
        log3 << "Min KNN Query Accuracy: " << std::to_string(min_knn_query_accuracy) << "\n";
        log3 << "Max Trajectories In Batch: " << std::to_string(max_trajectories_in_batch) << "\n";
        log3 << "Max Threads: " << std::to_string(max_threads) << "\n";
        log3 << "Range Query Grid Density: " << std::to_string(range_query_grid_density) << "\n";
        log3 << "KNN Query Grid Density: " << std::to_string(knn_query_grid_density) << "\n";
        log3 << "Windows Per Grid Point: " << std::to_string(windows_per_grid_point) << "\n";
        log3 << "Window Expansion Rate: " << std::to_string(window_expansion_rate) << "\n";
        log3 << "Range Query Time Interval Multiplier: " << std::to_string(range_query_time_interval_multiplier) << "\n";
        log3 << "Range KNN Time Interval Multiplier: " << std::to_string(knn_query_time_interval_multiplier) << "\n";
        log3 << "KNN K: " << std::to_string(custom_knn_k) << "\n";
        log3 << "Use KNN For Query Accuracy: " << std::to_string(use_KNN_for_query_accuracy) << "\n";
        log3 << "Benchmark:\n";
        log3 << "Runtime: " << time3 / 1000 << " s\n";
        log3 << "Range Query Accuracy: " << query_accuracy3.range_f1 << "\n";
        log3 << "KNN Query Accuracy: " << query_accuracy3.knn_f1 << "\n";
        log3 << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
        logger << log3.str();

        // Teardown
        trajectory_data_handling::Trajectory_Manager::reset_simplified_data();

    }

    void TRACE_Q_Benchmark::traceq_hardcore_query_accuracy(int amount_of_test_trajectories, logging::Logger & logger) {

        std::cout << "TRACE-Q Hardcore Query Accuracy" << std::endl;

        auto tests_per_min_accuracy = 3;

        auto custom_min_range_query_accuracy = 0.98;
        auto custom_min_knn_query_accuracy = 0.98;

        TRACE_Q_Benchmark::run_traceq_hardcore_benchmark(custom_min_range_query_accuracy, custom_min_knn_query_accuracy,
                                                         amount_of_test_trajectories, tests_per_min_accuracy, logger);

        custom_min_range_query_accuracy = 0.95;
        custom_min_knn_query_accuracy = 0.95;
        TRACE_Q_Benchmark::run_traceq_hardcore_benchmark(custom_min_range_query_accuracy, custom_min_knn_query_accuracy,
                                                         amount_of_test_trajectories, tests_per_min_accuracy, logger);

        custom_min_range_query_accuracy = 0.90;
        custom_min_knn_query_accuracy = 0.90;
        TRACE_Q_Benchmark::run_traceq_hardcore_benchmark(custom_min_range_query_accuracy, custom_min_knn_query_accuracy,
                                                         amount_of_test_trajectories, tests_per_min_accuracy, logger);

        custom_min_range_query_accuracy = 0.80;
        custom_min_knn_query_accuracy = 0.80;
        TRACE_Q_Benchmark::run_traceq_hardcore_benchmark(custom_min_range_query_accuracy, custom_min_knn_query_accuracy,
                                                         amount_of_test_trajectories, tests_per_min_accuracy, logger);

        custom_min_range_query_accuracy = 0.70;
        custom_min_knn_query_accuracy = 0.70;
        TRACE_Q_Benchmark::run_traceq_hardcore_benchmark(custom_min_range_query_accuracy, custom_min_knn_query_accuracy,
                                                         amount_of_test_trajectories, tests_per_min_accuracy, logger);
    }

    void TRACE_Q_Benchmark::run_traceq_hardcore_benchmark(double min_range_query_accuracy, double min_knn_query_accuracy,
                                                          int amount_of_test_trajectories, int tests_per_accuracy,
                                                          logging::Logger & logger) {
        for (int i = 0; i < tests_per_accuracy; ++i) {
            trajectory_data_handling::Trajectory_Manager::reset_all_data();
            trajectory_data_handling::File_Manager::load_tdrive_dataset(amount_of_test_trajectories);

            const auto query_objects = analytics::Benchmark::initialize_query_objects();

            double resolution_scale = 1.1;
            //double min_range_query_accuracy = 0.95;
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
            log << "Run " << i << ", Minimum Accuracy: " << min_range_query_accuracy << "\n";
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
        }
    }


    void TRACE_Q_Benchmark::mrpa_benchmark(int amount_of_test_trajectories, logging::Logger & logger) {
        std::cout << "TEST CASE: MRPA benchmarking" << std::endl;

        auto tests_per_min_accuracy = 3;
        double resolution_scale = 1.1;

        auto mrpa = simp_algorithms::MRPA{resolution_scale};

        auto mrpa_error = 0.999;
        run_mrpa_benchmark(mrpa_error, amount_of_test_trajectories, tests_per_min_accuracy, mrpa,
                           resolution_scale, logger);

        mrpa_error = 0.998;
        run_mrpa_benchmark(mrpa_error, amount_of_test_trajectories, tests_per_min_accuracy, mrpa,
                           resolution_scale, logger);

        mrpa_error = 0.9965;
        run_mrpa_benchmark(mrpa_error, amount_of_test_trajectories, tests_per_min_accuracy, mrpa,
                           resolution_scale, logger);

        mrpa_error = 0.995;
        run_mrpa_benchmark(mrpa_error, amount_of_test_trajectories, tests_per_min_accuracy, mrpa,
                           resolution_scale, logger);

        mrpa_error = 0.994;
        run_mrpa_benchmark(mrpa_error, amount_of_test_trajectories, tests_per_min_accuracy, mrpa,
                           resolution_scale, logger);

    }


    void TRACE_Q_Benchmark::run_mrpa(simp_algorithms::MRPA mrpa, std::vector<unsigned int> const & all_ids, double mrpa_error) {
        for (const auto& id : all_ids) {
            auto trajectory = trajectory_data_handling::Trajectory_Manager::load_into_data_structure(trajectory_data_handling::db_table::original_trajectories, {id}).front();
            if (trajectory.size() <= 2) {
                trajectory_data_handling::Trajectory_Manager::insert_trajectory(trajectory, trajectory_data_handling::db_table::simplified_trajectories);
                continue;
            }

            auto simplifications = mrpa.run_get_error_tolerances(trajectory);
            std::ranges::reverse(simplifications);
            for (const auto& [simp, error_tol] : simplifications) {
                std::cout << simp.size() << " " << error_tol << std::endl;

                if (error_tol > mrpa_error) {
                    trajectory_data_handling::Trajectory_Manager::insert_trajectory(simp, trajectory_data_handling::db_table::simplified_trajectories);
                    break;
                }
            }
            trajectory_data_handling::Trajectory_Manager::insert_trajectory(trajectory, trajectory_data_handling::db_table::simplified_trajectories);
        }
    }

    void TRACE_Q_Benchmark::run_mrpa_benchmark(double mrpa_error, int amount_of_test_trajectories,
                                               int tests_per_accuracy, simp_algorithms::MRPA const& mrpa,
                                               double resolution_scale, logging::Logger & logger) {
        for (int i = 0; i < tests_per_accuracy; ++i) {
            trajectory_data_handling::Trajectory_Manager::reset_all_data();
            trajectory_data_handling::File_Manager::load_tdrive_dataset(amount_of_test_trajectories);

            const auto query_objects = analytics::Benchmark::initialize_query_objects();

            auto all_ids = trajectory_data_handling::Trajectory_Manager::db_get_all_trajectory_ids(trajectory_data_handling::db_table::original_trajectories);

            auto time = analytics::Benchmark::function_time([&mrpa, &all_ids, mrpa_error]() {run_mrpa(mrpa, all_ids, mrpa_error); });
            auto query_accuracy = analytics::Benchmark::benchmark_query_accuracy(query_objects);

            std::stringstream log;

            log << "MRPA - Run " << i << ", Error = " << mrpa_error << "\n";
            log << "Resolution Scale: " << std::to_string(resolution_scale) << "\n";
            log << "Benchmark:\n";
            log << "Runtime: " << time / 1000 << " s\n";
            log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
            log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
            log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
            logger << log.str();
        }
    }

} // analytics
