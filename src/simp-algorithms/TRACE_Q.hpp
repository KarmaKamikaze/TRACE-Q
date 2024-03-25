#ifndef TRACE_Q_TRACE_Q_HPP
#define TRACE_Q_TRACE_Q_HPP

#include "../data/trajectory_structure.hpp"
#include "../querying/Query.hpp"

namespace trace_q {

    class TRACE_Q {
        /**
         * Total amount of queries to perform when checking query accuracy
         */
        int query_amount{};
        /**
         * The factor with which we will scale the grid.
         * The grid is defined by the highest and lowest values of latitude and longitude for a given trajectory.
         * Possible values: 1, 1.3
         */
        double grid_expansion_multiplier{};
        /**
         * Factor describing how close points appear in the grid.
         * Lower values cause points to be more densely packed, thus increasing the number of queries
         * Possible values: 0.2, 0.05
         */
        double grid_density_multiplier{};
        /**
         * Number of windows we will create for each grid point.
         * Only relevant to range queries, as these use windows
         * Increases to this value will increase the number of queries.
         */
        int windows_per_grid_point{};
        /**
         * The factor which the different windows of a grid point will scale.
         * This factor will be applied to the previous window's longitude and latitude to create the new window
         * The first window will have longitude and latitude corresponding to the grid density multiplier.
         * The first window for a grid point can also be smaller, i.e use the expansion rate reversely.
         * Possible values: 1.3, 2
         */
        double window_expansion_rate{};
        /**
         * The factor that will be used to scale time intervals for each window.
         * Lower values will result in more queries.
         * Possible values: 0.1, 1
         */
        double time_interval_multiplier{};
        /**
         * The percentage split between range queries and knn queries
         * Possible values: 60% (60% range queries and 40% knn queries)
         */

        [[nodiscard]] int calculate_query_amount() const;

        [[nodiscard]] double query_error(data_structures::Trajectory const& trajectory,
                             std::vector<std::shared_ptr<spatial_queries::Query>> const& query_objects) const;

    public:
        /**
         * The TRACE_Q constructor that determines the query_amount based on the given parameters.
         * @param grid_expansion_multiplier The grid scaling factor.
         * @param grid_density_multiplier The grid density factor, which describes how close points
         * appear in the grid.
         * @param windows_per_grid_point The amount of windows per point in the grid.
         * @param window_expansion_rate The window scaling rate for each grid point.
         * @param time_interval_multiplier The multiplier used to scale the time interval for each window.
         */
        TRACE_Q(double grid_expansion_multiplier, double grid_density_multiplier, int windows_per_grid_point,
                double window_expansion_rate, double time_interval_multiplier)
                : grid_expansion_multiplier(grid_expansion_multiplier),
                grid_density_multiplier(grid_density_multiplier),
                windows_per_grid_point(windows_per_grid_point),
                window_expansion_rate(window_expansion_rate),
                time_interval_multiplier(time_interval_multiplier) {
            query_amount = calculate_query_amount();
        }

        [[nodiscard]] data_structures::Trajectory simplify(const data_structures::Trajectory& original_trajectory,
                                             double max_locations, double min_query_accuracy) const;

    };

} // trace_q

#endif //TRACE_Q_TRACE_Q_HPP
