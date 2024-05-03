#include <doctest/doctest.h>
#include "../simp-algorithms/TRACE_Q.hpp"
#include "../trajectory_data_handling/Trajectory_Manager.hpp"
#include "../trajectory_data_handling/File_Manager.hpp"
#include "Benchmark.hpp"
#include "../logging/Logger.hpp"

TEST_CASE("TRACE_Q Window expansion rate benchmarking") {

    trajectory_data_handling::Trajectory_Manager::reset_all_data();
    trajectory_data_handling::File_Manager::load_tdrive_dataset();

    // Get the current time
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    // Convert the current time to a formatted string (YYYYMMDD_HHMMSS)
    std::stringstream time_ss;
    time_ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    std::string timestamp = time_ss.str();
    auto logger = logging::Logger{"../../logs", "/" + timestamp + ".txt"};

    auto query_objects = analytics::Benchmark::initialize_query_objects();

    double resolution_scale = 1.3;
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

        log << "TRACE-Q Runtime vs Query Accuracy - Window expansion rate = 1.2" << "\n";
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

        log << "TRACE-Q Runtime vs Query Accuracy - Window expansion rate = 1.3" << "\n";
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

        log << "TRACE-Q Runtime vs Query Accuracy - Window expansion rate = 1.5" << "\n";
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

    // Get the current time
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    // Convert the current time to a formatted string (YYYYMMDD_HHMMSS)
    std::stringstream time_ss;
    time_ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    std::string timestamp = time_ss.str();
    auto logger = logging::Logger{"../../logs", "/" + timestamp + ".txt"};

    auto query_objects = analytics::Benchmark::initialize_query_objects();

    double resolution_scale = 2.0;
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

        log << "TRACE-Q Runtime vs Query Accuracy - KNN K = 1" << "\n";
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

        log << "TRACE-Q Runtime vs Query Accuracy - KNN K = 10" << "\n";
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

        log << "TRACE-Q Runtime vs Query Accuracy - KNN K = 50" << "\n";
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

TEST_CASE("TRACE-Q IS KNN NECESSARY?") {

    /*
     * The TRACE-Q benchmark tests assume that the dataset consist of 50 trajectories.
     */

    trajectory_data_handling::Trajectory_Manager::reset_all_data();
    trajectory_data_handling::File_Manager::load_tdrive_dataset();

    // Get the current time
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    // Convert the current time to a formatted string (YYYYMMDD_HHMMSS)
    std::stringstream time_ss;
    time_ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    std::string timestamp = time_ss.str();
    auto logger = logging::Logger{"../../logs", "/" + timestamp + ".txt"};

    auto query_objects = analytics::Benchmark::initialize_query_objects();

    double resolution_scale = 1.3;
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

        double custom_min_range_query_accuracy = 0.86;
        double custom_min_knn_query_accuracy = 0.89;
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

        log << "TRACE-Q WITH KNN" << "\n";
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

        log << "TRACE-Q WITHOUT KNN" << std::endl;
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

TEST_CASE("TRACE-Q VS RL4QDTS") {

    trajectory_data_handling::Trajectory_Manager::reset_all_data();
    trajectory_data_handling::File_Manager::load_tdrive_dataset();

    // Get the current time
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    // Convert the current time to a formatted string (YYYYMMDD_HHMMSS)
    std::stringstream time_ss;
    time_ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    std::string timestamp = time_ss.str();
    auto logger = logging::Logger{"../../logs", "/" + timestamp + ".txt"};

    auto query_objects = analytics::Benchmark::evil_initialize_query_objects(); // This is where the magic happens

    double resolution_scale = 1.3;
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

    log << "TRACE-Q VS RL4QDTS" << "\n";
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

    // Get the current time
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    // Convert the current time to a formatted string (YYYYMMDD_HHMMSS)
    std::stringstream time_ss;
    time_ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    std::string timestamp = time_ss.str();
    auto logger = logging::Logger{"../../logs", "/" + timestamp + ".txt"};

    auto query_objects = analytics::Benchmark::initialize_query_objects();

    double resolution_scale = 1.3;
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

    log << "TRACE-Q Hardcore mode (Enemies deal more damage, your hunger depletes faster, query accuracy is very unforgiving, and your save file is deleted if you die.)" << "\n";
    log << "Runtime: " << time / 1000 << " s\n";
    log << "Range Query Accuracy: " << query_accuracy.range_f1 << "\n";
    log << "KNN Query Accuracy: " << query_accuracy.knn_f1 << "\n";
    log << "Compression Ratio: " << analytics::Benchmark::get_compression_ratio() << "\n";
    logger << log.str();

    // Teardown
    trajectory_data_handling::Trajectory_Manager::reset_all_data();
    trajectory_data_handling::File_Manager::load_tdrive_dataset();
}
