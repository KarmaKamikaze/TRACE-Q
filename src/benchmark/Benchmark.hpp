#ifndef TRACE_Q_BENCHMARK_HPP
#define TRACE_Q_BENCHMARK_HPP

#include <chrono>
#include <functional>
#include "../querying/KNN_Query.hpp"
#include "../simp-algorithms/TRACE_Q.hpp"
#include "F1.hpp"
#include "benchmark-query-objects/Benchmark_Query.hpp"
#include "benchmark-query-objects/Benchmark_Range_Query.hpp"
#include "benchmark-query-objects/Benchmark_KNN_Query.hpp"

namespace analytics {

    struct Query_Accuracy {
        double range_f1{};
        double knn_f1{};
    };

    struct MBR {
        double x_low{};
        double x_high{};
        double y_low{};
        double y_high{};
        unsigned long t_low{};
        unsigned long t_high{};
    };

    class Benchmark {
    private:
        static std::string connection_string;
    public:
        static int max_connections;
        static double expansion_factor;
        static double grid_density;
        static int windows_per_grid_point;
        static double window_expansion_rate;
        static double time_interval;
        static int knn_k;

        static MBR get_mbr();
        static std::vector<std::shared_ptr<Benchmark_Query>> initialize_query_objects();
        static Query_Accuracy benchmark_query_accuracy(std::vector<std::shared_ptr<Benchmark_Query>> const& query_objects);
        static std::vector<std::future<Benchmark_Range_Query>> create_range_query_futures(double x, double y, unsigned long t, MBR const& mbr);
        static std::future<Benchmark_KNN_Query> create_knn_query_future(int k, double x, double y, unsigned long t, MBR const& mbr);
        static std::pair<double, double> calculate_window_range(
                double center, double mbr_low, double mbr_high, double w_expansion_rate,
                double w_grid_density, int window_number);
        static std::pair<unsigned long, unsigned long> calculate_time_range(
                unsigned long center, unsigned long mbr_low, unsigned long mbr_high,
                double w_expansion_rate, double w_grid_density, int window_number);
        static double get_compression_ratio();

        /**
         * This function takes a function and its parameters and times how long times it takes to execute it.
         * This is recorded in milliseconds.
         * @tparam F
         * @tparam Args
         * @param func The function that is measured
         * @param args The measured functions parameters.
         * @return The duration of the input function. Endtime-starttime.
         */
            template<typename F, typename... Args>
            static long long int function_time(F&& func, Args&&... args) {
                std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
                std::invoke(std::forward<F>(func), std::forward<Args>(args)...);
                return std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::high_resolution_clock::now()-t1
                ).count();
            }
    };

}
#endif //TRACE_Q_BENCHMARK_HPP
