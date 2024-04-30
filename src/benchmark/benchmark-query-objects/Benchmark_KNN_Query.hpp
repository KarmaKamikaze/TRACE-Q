#ifndef TRACE_Q_BENCHMARK_KNN_QUERY_HPP
#define TRACE_Q_BENCHMARK_KNN_QUERY_HPP

#include "Benchmark_Query.hpp"
#include "../../querying/KNN_Query.hpp"

namespace analytics {

    class Benchmark_KNN_Query : public Benchmark_Query {
    public:
        Benchmark_KNN_Query(std::unordered_set<unsigned int> const& query_result_ids,
                            spatial_queries::KNN_Query::KNN_Origin const& origin)
        : Benchmark_Query(query_result_ids), origin(origin) {}
        spatial_queries::KNN_Query::KNN_Origin origin{};
        ~Benchmark_KNN_Query() override = default;
    };

}

#endif //TRACE_Q_BENCHMARK_KNN_QUERY_HPP
