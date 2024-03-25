#ifndef TRACE_Q_QUERY_H
#define TRACE_Q_QUERY_H


#include "../data/trajectory_structure.hpp"

namespace spatial_queries {
    struct Query {

        virtual bool operator()(data_structures::Trajectory const&) = 0;
        virtual ~Query() = default;

    };

} // spatial_queries

#endif //TRACE_Q_QUERY_H
