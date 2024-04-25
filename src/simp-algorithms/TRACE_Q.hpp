#ifndef TRACE_Q_TRACE_Q_HPP
#define TRACE_Q_TRACE_Q_HPP

#include <future>
#include <cmath>
#include "../data/Trajectory.hpp"
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
         * The minimum query accuracy that the simplification method must uphold.
         */
        double min_query_accuracy{};

        /**
         * The maximum number of trajectories per batch that is able to run concurrently due to database connections.
         */
        int max_trajectories_in_batch{};

        /**
         * The factor with which we will scale the grid for range queries.
         * Calculated based on the range_query_grid_density_multiplier.
         * The grid is defined by the highest and lowest values of latitude and longitude for a given trajectory.
         * Possible values: 0.2, 0.43
         */
        double range_query_grid_expansion_factor{};

        /**
         * The factor with which we will scale the grid for get_ids_from_knn queries.
         * Calculated based on the knn_query_grid_density_multiplier.
         * The grid is defined by the highest and lowest values of latitude and longitude for a given trajectory.
         * Possible values: 0.2, 0.43
         */
        double knn_query_grid_expansion_factor{};

        /**
         * Factor describing how close points appear in the grid for range queries.
         * Lower values cause points to be more densely packed, thus increasing the number of queries
         * Possible values: 0.2, 0.05
         */
        double range_query_grid_density{};

        /**
         * Factor describing how close points appear in the grid for get_ids_from_knn queries.
         * Lower values cause points to be more densely packed, thus increasing the number of queries
         * Possible values: 0.2, 0.05
         */
        double knn_query_grid_density{};

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
         * The factor that will be used to scale time intervals for each window in range queries.
         * Lower values will result in more queries.
         * Possible values: 0.1, 1
         */
        double range_query_time_interval_multiplier{};

        /**
         * The factor that will be used to scale time intervals for each window in get_ids_from_knn queries.
         * Lower values will result in more queries.
         * Possible values: 0.1, 1
         * Lowest allowed KNN time interval multiplier is 0.02, otherwise number of query time points would be larger
         * than the allowed database connections.
         */
        double knn_query_time_interval_multiplier{};

        /**
         * Number of nearest neighbours to analyse.
         */
        int knn_k{};

        /**
         * Decides whether KNN queries should be utilized for determining the query accuracy.
         */
        bool use_KNN_for_query_accuracy{};


        /**
         * Calculates the query error of a simplified trajectory on a set of query objects.
         * This is more accurately a query accuracy, since it is a percentage queries that return correct results.
         * @param trajectory Simplified trajectory
         * @param query_objects Vector of query objects that define a query and contain the original trajectory's result
         * @return Query accuracy
         */
        [[nodiscard]] double query_accuracy(
                data_structures::Trajectory const& trajectory,
                std::vector<std::shared_ptr<spatial_queries::Query>> const& query_objects) const;

        /**
         * A Minimum Bounding Rectangle for trajectory data.
         */
        struct MBR {
            double x_low{};
            double x_high{};
            double y_low{};
            double y_high{};
            unsigned long t_low{};
            unsigned long t_high{};
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
        static MBR expand_MBR(MBR mbr, double expansion_factor);

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
                data_structures::Trajectory const& trajectory, double x, double y, unsigned long t, MBR const& mbr) const;

        /**
         * Initializes a list of KNN query objects with the given trajectory used to perform query similarity between
         * KNN queries containing the original trajectory and the simplified counter-part.
         * @param trajectory The original trajectory for which range queries will be performed.
         * @param x The x-axis grid point.
         * @param y The y-axis grid point.
         * @param t The t-axis grid point.
         * @param mbr The Minimum Bounding Rectangle that encompasses the Trajectory.
         * @return A shared pointer to a KNN query object that have been initialized with the given trajectory.
         */
        static std::shared_ptr<spatial_queries::KNN_Query_Test> knn_query_initialization(
                unsigned int original_trajectory_id,
                double x, double y, unsigned long t, MBR const& mbr, double time_interval_multiplier, int knn_k);

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
         template<typename T>
        static std::pair<T, T> calculate_window_range(
                T center, T mbr_low, T mbr_high,
                double window_expansion_rate, double grid_density, int window_number);

        /**
         * Creates and initializes query test function objects for the given trajectory.
         * @param original_trajectory The trajectory for which query tests will be created.
         * @return A list of query tests.
         */
        [[nodiscard]] std::vector<std::shared_ptr<spatial_queries::Query>> initialize_query_tests(
                data_structures::Trajectory const& original_trajectory) const;

        static int process_futures(std::vector<std::future<bool>>& futures);

        /**
         * Runs a single batch of the TRACE-Q algorithm given the list of trajectory IDs.
         * The original trajectories are fetched from the database, simplified using the TRACE-Q algorithm which
         * utilizes MRPA, and then inserted into the database for later querying. The batch is run concurrently for
         * each trajectory.
         * @param ids The list of trajectory IDs for which the batch job will run.
         */
        void batch_job(std::vector<unsigned int> const& ids) const;

        [[nodiscard]] data_structures::Trajectory simplify(const data_structures::Trajectory& original_trajectory) const;

    public:
        /**
         * The TRACE_Q constructor that determines the query_amount based on the given parameters.
         * @param resolution_scale The MRPA resolution scale.
         * @param min_query_accuracy The minimum query accuracy that the simplification method must uphold.
         * @param range_query_grid_density_multiplier The grid density factor for range queries,
         * which describes how close points appear in the grid.
         * @param windows_per_grid_point The amount of windows per point in the grid.
         * @param window_expansion_rate The window scaling rate for each grid point.
         * @param range_query_time_interval_multiplier The multiplier used to scale the time interval for each window
         * in range queries.
         * @param use_KNN_for_query_accuracy Decides whether KNN queries should be utilized for determining query accuracy.
         */
        TRACE_Q(double resolution_scale, double min_query_accuracy, double range_query_grid_density_multiplier,  int windows_per_grid_point,
                double window_expansion_rate, double range_query_time_interval_multiplier, bool use_KNN_for_query_accuracy)
                : mrpa(resolution_scale),
                  min_query_accuracy(min_query_accuracy),
                  max_trajectories_in_batch(5),
                  range_query_grid_expansion_factor(range_query_grid_density_multiplier * 0.8),
                  range_query_grid_density(range_query_grid_density_multiplier),
                  windows_per_grid_point(windows_per_grid_point),
                  window_expansion_rate(window_expansion_rate),
                  range_query_time_interval_multiplier(range_query_time_interval_multiplier),
                  use_KNN_for_query_accuracy(use_KNN_for_query_accuracy) {}

        /**
         * The TRACE_Q constructor, which includes KNN queries, that determines the query_amount based on the given parameters.
         * @param resolution_scale The MRPA resolution scale.
         * @param min_query_accuracy The minimum query accuracy that the simplification method must uphold.
         * @param range_query_grid_density_multiplier The grid density factor for range queries,
         * which describes how close points appear in the grid.
         * @param knn_query_grid_density_multiplier The grid density factor for KNN queries,
         * which describes how close points appear in the grid.
         * @param windows_per_grid_point The amount of windows per point in the grid.
         * @param window_expansion_rate The window scaling rate for each grid point.
         * @param range_query_time_interval_multiplier The multiplier used to scale the time interval for each window
         * in range queries.
         * @param knn_query_time_interval_multiplier The multiplier used to scale the time interval for each window
         * in KNN queries.
         * @param knn_k The K value for K-Nearest-Neighbour queries.
         * @param use_KNN_for_query_accuracy Decides whether KNN queries should be utilized for determining query accuracy.
         */
        TRACE_Q(double resolution_scale, double min_query_accuracy, double range_query_grid_density_multiplier,
                double knn_query_grid_density_multiplier,  int windows_per_grid_point,
                double window_expansion_rate, double range_query_time_interval_multiplier,
                double knn_query_time_interval_multiplier, int knn_k, bool use_KNN_for_query_accuracy)
                : mrpa(resolution_scale),
                  min_query_accuracy(min_query_accuracy),
                  range_query_grid_expansion_factor(range_query_grid_density_multiplier * 0.8),
                  knn_query_grid_expansion_factor(knn_query_grid_density_multiplier * 0.8),
                  range_query_grid_density(range_query_grid_density_multiplier),
                  knn_query_grid_density(knn_query_grid_density_multiplier),
                  windows_per_grid_point(windows_per_grid_point),
                  window_expansion_rate(window_expansion_rate),
                  range_query_time_interval_multiplier(range_query_time_interval_multiplier),
                  knn_query_time_interval_multiplier(knn_query_time_interval_multiplier),
                  knn_k(knn_k),
                  use_KNN_for_query_accuracy(use_KNN_for_query_accuracy){
            if (knn_query_time_interval_multiplier < 0.02) {
                throw std::invalid_argument("knn_query_time_interval_multiplier was lower than allowed");
            }
            max_trajectories_in_batch = static_cast<int>(std::floor(80 / (1 / knn_query_time_interval_multiplier)));
        }

        /**
         * Runs the TRACE-Q algorithm in batches with MRPA.
         */
        void run() const;

    };

} // trace_q

#endif //TRACE_Q_TRACE_Q_HPP
