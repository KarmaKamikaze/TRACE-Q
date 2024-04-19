#ifndef TRACE_Q_RANGE_QUERY_HPP
#define TRACE_Q_RANGE_QUERY_HPP


#include "../data/Trajectory.hpp"
#include <limits>

namespace spatial_queries {

    struct Range_Query {
        struct Window {
            double x_low{ std::numeric_limits<double>::min() };
            double x_high{ std::numeric_limits<double>::max() };
            double y_low{ std::numeric_limits<double>::min() };
            double y_high{ std::numeric_limits<double>::max() };
            unsigned long t_low{ std::numeric_limits<unsigned long>::min() };
            unsigned long t_high{ std::numeric_limits<unsigned long>::max() };
        };

        /**
         * Determines whether the given trajectory is in the window.
         * @param trajectory Trajectory to check whether is in the window.
         * @param window The window wherein the trajectory is tested for presence.
         * @return A boolean value determining whether the given trajectory is in the window.
         */
        static bool in_range(data_structures::Trajectory const& trajectory, Window const& window);

        /**
         * Performs a range query on the given database given a window.
         * @param table The table to query.
         * @param window The window wherein the trajectories are tested for presence.
         * @return A list of trajectory IDs corresponding to the range query using the given window.
         */
        static std::vector<unsigned int> get_ids_from_range_query(std::string const& table, Window const& window);

    private:
        /**
         * The connection string that specifies the connection details for the PostgreSQL database.
         */
        static std::string connection_string;
    };

} // spatial_queries


#endif //TRACE_Q_RANGE_QUERY_HPP
