#ifndef TRACE_Q_RANGE_QUERY_HPP
#define TRACE_Q_RANGE_QUERY_HPP


#include "../data/Trajectory.hpp"
#include <limits>

namespace spatial_queries {

    struct Range_Query {
        struct Window {
            double x_low{ std::numeric_limits<double>::lowest() };
            double x_high{ std::numeric_limits<double>::max() };
            double y_low{ std::numeric_limits<double>::lowest() };
            double y_high{ std::numeric_limits<double>::max() };
            long t_low{ std::numeric_limits<long>::lowest() };
            long t_high{ std::numeric_limits<long>::max() };
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
