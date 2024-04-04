#ifndef TRACE_Q_TRACE_Q_HPP
#define TRACE_Q_TRACE_Q_HPP

#include "../data/trajectory_structure.hpp"
#include "../querying/Query.hpp"
#include "../querying/Range_Query_Test.hpp"
#include "../querying/KNN_Query_Test.hpp"
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
         * The factor with which we will scale the grid. Calculated based on the grid_density_multiplier.
         * The grid is defined by the highest and lowest values of latitude and longitude for a given trajectory.
         * Possible values: 0.2, 0.43
         */
        double grid_expansion_factor{};

        /**
         * Factor describing how close points appear in the grid.
         * Lower values cause points to be more densely packed, thus increasing the number of queries
         * Possible values: 0.2, 0.05
         */
        double grid_density{};

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
        [[nodiscard]] double query_accuracy(data_structures::Trajectory const& trajectory,
                                            std::vector<std::shared_ptr<spatial_queries::Query>> const& query_objects) const;

        /**
         * A Minimum Bounding Rectangle for trajectory data.
         */
        struct MBR {
            double x_low{};
            double x_high{};
            double y_low{};
            double y_high{};
            long double t_low{};
            long double t_high{};
        };

        /**
         * Calculates the Minimum Bounding Rectangle for a given trajectory.
         * @param trajectory The Trajectory for which to calculate the MBR.
         * @return The resultant MBR encompassing the trajectory.
         */
        static MBR calculate_MBR(data_structures::Trajectory const& trajectory);

        /**
         * Expands the Minimum Bounding Rectangle according to the grid expansion factor.
         * @param mbr The MBR which will be expanded.
         * @return The expanded MBR.
         */
        [[nodiscard]] MBR expand_MBR(MBR mbr) const;

        /**
         * Initializes a list of range query objects with the given trajectory used for the original_in_window boolean.
         * @param trajectory The original trajectory for which range queries will be performed.
         * @param x The x-axis grid point.
         * @param y The y-axis grid point.
         * @param t The t-axis grid point.
         * @param mbr The Minimum Bounding Rectangle that encompasses the Trajectory.
         * @return A list of shared pointers to a number of range query objects that have been initialized with the given trajectory.
         */
        [[nodiscard]] std::vector<std::shared_ptr<spatial_queries::Range_Query_Test>> range_query_initialization(
                data_structures::Trajectory const& trajectory, double x, double y, long double t, MBR const& mbr) const;

        /**
         * This function calculates the lower and upper bounds of a range centered around a given value,
         * considering the window expansion rate, grid density, and window number.
         *
         * @param center The center value around which the range is calculated.
         * @param mbr_low The lower bound of the Minimum Bounding Rectangle (MBR) for the dimension.
         * @param mbr_high The upper bound of the Minimum Bounding Rectangle (MBR) for the dimension.
         * @param window_expansion_rate The rate at which the window expands with each window number.
         * @param grid_density The density of the grid, affecting the size of the window.
         * @param window_number The number of the window for which the range is calculated.
         * @return A pair containing the lower and upper bounds of the calculated range.
         */
        static std::pair<long double, long double> calculate_window_range(
                long double center, long double mbr_low, long double mbr_high,
                double window_expansion_rate, double grid_density, int window_number);

        [[nodiscard]] std::vector<std::shared_ptr<spatial_queries::Query>> initialize_query_tests(
                data_structures::Trajectory const& original_trajectory) const;

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
                  grid_expansion_factor(grid_density_multiplier * 0.8),
                  grid_density(grid_density_multiplier),
                  windows_per_grid_point(windows_per_grid_point),
                  window_expansion_rate(window_expansion_rate),
                  time_interval_multiplier(time_interval_multiplier) {
            query_amount = calculate_query_amount();
        }


        [[nodiscard]] data_structures::Trajectory simplify(const data_structures::Trajectory& original_trajectory,
                                             double min_query_accuracy) const;

    };

} // trace_q

#endif //TRACE_Q_TRACE_Q_HPP
