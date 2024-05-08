#ifndef TRACE_Q_TRACE_Q_BENCHMARK_HPP
#define TRACE_Q_TRACE_Q_BENCHMARK_HPP

#include <memory>
#include "benchmark-query-objects/Benchmark_Query.hpp"
#include "../logging/Logger.hpp"
#include "../simp-algorithms/MRPA.hpp"

namespace analytics {

    struct TRACE_Q_Benchmark {
        static void run_traceq_benchmarks();
        static void traceq_is_knn_neccesary(std::vector<std::shared_ptr<Benchmark_Query>> const& query_objects,
                                            logging::Logger & logger);
        static void traceq_window_expansion_rate(std::vector<std::shared_ptr<Benchmark_Query>> const& query_objects,
                                                 logging::Logger & logger);
        static void traceq_range_query_density_and_time_interval(std::vector<std::shared_ptr<Benchmark_Query>> const& query_objects,
                                                                 logging::Logger & logger);
        static void traceq_knn_query_density_and_time_interval(std::vector<std::shared_ptr<Benchmark_Query>> const& query_objects,
                                                               logging::Logger & logger);
        static void traceq_knn_k(std::vector<std::shared_ptr<Benchmark_Query>> const& query_objects,
                                 logging::Logger & logger);
        static void traceq_hardcore_query_accuracy(std::vector<std::shared_ptr<Benchmark_Query>> const& query_objects,
                                                   logging::Logger & logger);
        static void run_mrpa(simp_algorithms::MRPA mrpa, std::vector<unsigned int> const & all_ids, double mrpa_error);
        static void mrpa_benchmark(std::vector<std::shared_ptr<Benchmark_Query>> const& query_objects,
                                               logging::Logger & logger);
    };

} // analytics

#endif //TRACE_Q_TRACE_Q_BENCHMARK_HPP
