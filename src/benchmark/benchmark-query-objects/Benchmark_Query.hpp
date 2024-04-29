#ifndef TRACE_Q_BENCHMARK_QUERY_HPP
#define TRACE_Q_BENCHMARK_QUERY_HPP

#include <unordered_set>

namespace analytics {

    struct Benchmark_Query {
        explicit Benchmark_Query(std::unordered_set<unsigned int> const& query_result_ids) : query_result_ids(query_result_ids) {}
        std::unordered_set<unsigned int> query_result_ids{};
        virtual ~Benchmark_Query() = default;
    };

}

#endif //TRACE_Q_BENCHMARK_QUERY_HPP
