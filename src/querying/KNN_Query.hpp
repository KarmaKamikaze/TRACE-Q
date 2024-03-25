#ifndef TRACE_Q_KNN_QUERY_HPP
#define TRACE_Q_KNN_QUERY_HPP

#include "Query.hpp"

namespace spatial_queries {

    class KNN_Query : public Query {

        bool operator()(data_structures::Trajectory const& trajectory) override;

        ~KNN_Query() override = default;
    };

} // spatial_queries

#endif //TRACE_Q_KNN_QUERY_HPP
