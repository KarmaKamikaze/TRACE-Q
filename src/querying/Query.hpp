#ifndef TRACE_Q_QUERY_H
#define TRACE_Q_QUERY_H


#include "../data/trajectory_structure.hpp"

namespace spatial_queries {
    struct Query {
        struct Window {
            double x_low{};
            double x_high{};
            double y_low{};
            double y_high{};
            double t_low{};
            double t_high{};
        };

        static bool in_range(data_structures::Trajectory const& trajectory, Window const& window);
        static double query_error(data_structures::Trajectory const& );
    };

} // spatial_queries

#endif //TRACE_Q_QUERY_H
