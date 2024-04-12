#ifndef TRACE_Q_QUERY_H
#define TRACE_Q_QUERY_H


#include "../data/Trajectory.hpp"

namespace spatial_queries {
    struct Query {

        /**
         * Function object method that determines if both the original and the simplified trajectories is in the query.
         * @return A boolean value representing if both original and simplified trajectories are in the query.
         */
        virtual bool operator()(data_structures::Trajectory const&) = 0;
        virtual ~Query() = default;

    };

} // spatial_queries

#endif //TRACE_Q_QUERY_H
