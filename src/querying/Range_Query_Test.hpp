#ifndef TRACE_Q_RANGE_QUERY_TEST_HPP
#define TRACE_Q_RANGE_QUERY_TEST_HPP

#include "Query.hpp"
#include "Range_Query.hpp"

namespace spatial_queries {

    class Range_Query_Test : public Query {
    private:
        /**
         * The window that describes the area of the range query.
         */
        Range_Query::Window window{};

    public:

        /**
         * Describes whether the original trajectory is in the window.
         */
        bool original_in_window{};

        /**
         * Range_Query constructor that determines if the original_trajectory is in the range query.
         * @param original_trajectory The non-simplified, original trajectory.
         * @param x_low The low x value of the range query.
         * @param x_high The high x value of the range query.
         * @param y_low The low y value of the range query.
         * @param y_high The high y value of the range query.
         * @param t_low The low time value of the range query.
         * @param t_high The high time value of the range query.
         */
        Range_Query_Test(const data_structures::Trajectory& original_trajectory, double x_low, double x_high, double y_low,
                         double y_high, unsigned long t_low, unsigned long t_high) :
                         window{x_low, x_high, y_low, y_high, t_low, t_high},
                         original_in_window{Range_Query::in_range(original_trajectory, window)} {}

        /**
         * Evaluates if the original and simplified trajectory return the same result when performing the range query.
         * @param trajectory Simplified trajectory.
         * @return A boolean value defining whether the range query when performed on the original and simplified
         * trajectories return the same.
         */
        bool operator()(data_structures::Trajectory const& trajectory) override;

        ~Range_Query_Test() override = default;

    };

} // spatial_queries

#endif //TRACE_Q_RANGE_QUERY_TEST_HPP
