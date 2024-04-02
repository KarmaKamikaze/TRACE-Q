#ifndef TRACE_Q_TRACE_Q_HPP
#define TRACE_Q_TRACE_Q_HPP

#include "../data/trajectory_structure.hpp"
#include "../querying/Query.hpp"
#include "../querying/Range_Query.hpp"
#include "../querying/KNN_Query.hpp"
#include "MRPA.hpp"

namespace trace_q {

    class TRACE_Q {
        /**
         * The MRPA algorithm as a function object.
         */
        simp_algorithms::MRPA mrpa{};

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
         * Calculates the amount of queries to be performed for a particular trajectory to determine the query error.
         * The amount is calculated by determining the amount of points in the query grid. The grid is then used
         * to decide the amount of queries based on the time interval multiplier in both the range and KNN query cases.
         * Additionally, in range queries, the windows-per-grid-point is taken into account.
         * @return The total amount of queries to be performed.
         */
        [[nodiscard]] int calculate_query_amount() const;

        /**
         * Calculates the query error of a simplified trajectory on a set of query objects.
         * This is more accurately a query accuracy, since it is a percentage queries that return correct results.
         * @param trajectory Simplified trajectory
         * @param query_objects Vector of query objects that define a query and contain the original trajectory's result
         * @return Query accuracy
         */
        [[nodiscard]] double query_error(data_structures::Trajectory const& trajectory,
                             std::vector<std::shared_ptr<spatial_queries::Query>> const& query_objects) const;

        struct MBR {
            double x_low{};
            double x_high{};
            double y_low{};
            double y_high{};
            long double t_low{};
            long double t_high{};
        };

        static MBR calculate_MBR(data_structures::Trajectory const&);

        MBR expand_MBR(MBR) const;

        [[nodiscard]] std::vector<std::shared_ptr<spatial_queries::Range_Query>> range_query_initialization(
                data_structures::Trajectory const& trajectory, double x, double y, long double t, MBR const& mbr) const;

    public:
        /**
         * The TRACE_Q constructor that determines the query_amount based on the given parameters.
         * @param resolution_scale The MRPA resolution scale.
         * @param grid_density_multiplier The grid density factor, which describes how close points
         * appear in the grid.
         * @param windows_per_grid_point The amount of windows per point in the grid.
         * @param window_expansion_rate The window scaling rate for each grid point.
         * @param time_interval_multiplier The multiplier used to scale the time interval for each window.
         */
        TRACE_Q(double resolution_scale, double grid_density_multiplier, int windows_per_grid_point,
                double window_expansion_rate, double time_interval_multiplier)
                : mrpa(resolution_scale),
                grid_expansion_multiplier(grid_density_multiplier * 0.8),
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
