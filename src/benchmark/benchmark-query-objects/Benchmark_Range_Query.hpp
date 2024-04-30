#ifndef TRACE_Q_BENCHMARK_RANGE_QUERY_HPP
#define TRACE_Q_BENCHMARK_RANGE_QUERY_HPP

#include "Benchmark_Query.hpp"
#include "../../querying/Range_Query.hpp"

namespace analytics {

    class Benchmark_Range_Query : public Benchmark_Query {
    public:
        Benchmark_Range_Query(std::unordered_set<unsigned int> const& query_result_ids,
                              spatial_queries::Range_Query::Window const& window)
        : Benchmark_Query(query_result_ids), window(window) {}
        spatial_queries::Range_Query::Window window{};
        ~Benchmark_Range_Query() override = default;
    };

} // analytics

#endif //TRACE_Q_BENCHMARK_RANGE_QUERY_HPP
