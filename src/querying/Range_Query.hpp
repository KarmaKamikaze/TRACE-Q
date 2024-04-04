#ifndef TRACE_Q_RANGE_QUERY_HPP
#define TRACE_Q_RANGE_QUERY_HPP


#include "../data/trajectory_structure.hpp"

namespace spatial_queries {

    struct Range_Query {
        struct Window {
            double x_low{};
            double x_high{};
            double y_low{};
            double y_high{};
            long double t_low{};
            long double t_high{};
        };

        /**
         * Determines whether the given trajectory is in the window.
         * @param trajectory Trajectory to check whether is in the window.
         * @return A boolean value determining whether the given trajectory is in the window.
         */
        static bool in_range(data_structures::Trajectory const& trajectory, Window const& window);
    };

} // spatial_queries


#endif //TRACE_Q_RANGE_QUERY_HPP
