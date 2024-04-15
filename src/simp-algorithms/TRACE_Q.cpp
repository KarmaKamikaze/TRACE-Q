#include <future>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <numeric>
#include "TRACE_Q.hpp"
#include "MRPA.hpp"

namespace trace_q {

    data_structures::Trajectory TRACE_Q::simplify(data_structures::Trajectory const& original_trajectory, double min_query_accuracy) const {
        auto simplifications = mrpa(original_trajectory);

        auto query_objects = initialize_query_tests(original_trajectory);


        // iterate from the back since simplifications appear in decreasing resolution
        for (int i = static_cast<int>(simplifications.size()) - 1; i >= 0; --i) {
            // TODO: Temporarily replace trajectory with id corresponding to original in the database with the simplified trajectory
            if (query_accuracy(simplifications[i], query_objects) >= min_query_accuracy) {
                // TODO: Reinsert original into the database and remove the simplified
                return simplifications[i];
            }
        }
        // TODO: Reinsert original into the database and remove the simplified
        return original_trajectory;
    }

    std::vector<std::shared_ptr<spatial_queries::Query>> TRACE_Q::initialize_query_tests(
            data_structures::Trajectory const& original_trajectory) const {
        std::vector<std::shared_ptr<spatial_queries::Query>> query_objects{};

        auto mbr = expand_MBR(calculate_MBR(original_trajectory));

        // Defines how many points there should be on both the x and y axes on the grid.
        auto points_on_axis = static_cast<int>(std::ceil(1 / grid_density));
        // Defines how many time intervals there should be made for each point in the grid.
        auto time_points = static_cast<int>(std::ceil(1 / time_interval_multiplier));

        for (int x_point = 0; x_point <= points_on_axis; ++x_point) {
            // Scale the coordinate based on the grid density, the mbr and the current point on the x-axis.
            auto x_coord = mbr.x_low + x_point * grid_density * (mbr.x_high - mbr.x_low);
            for (int y_point = 0; y_point <= points_on_axis; ++y_point) {
                auto y_coord = mbr.y_low + y_point * grid_density * (mbr.y_high - mbr.y_low);
                for (int t_point = 0; t_point <= time_points; ++t_point) {
                    auto t_interval = mbr.t_low + t_point * time_interval_multiplier * (mbr.t_high - mbr.t_low);

                    auto range_queries = range_query_initialization(original_trajectory,
                                                                    x_coord, y_coord, t_interval, mbr);
                    query_objects.insert(std::end(query_objects), std::begin(range_queries),
                                         std::end(range_queries));

                    // TODO: KNN
                }
            }
        }

        return query_objects;
    }

    double TRACE_Q::query_accuracy(data_structures::Trajectory const& trajectory,
                                   std::vector<std::shared_ptr<spatial_queries::Query>> const& query_objects) {

        std::vector<std::future<bool>> range_futures{};
        std::vector<std::future<bool>> knn_futures{};

        for (auto const& query_object : query_objects) {
            if (auto range_query = std::dynamic_pointer_cast<spatial_queries::Range_Query_Test>(query_object)) {
                range_futures.emplace_back(std::async(std::launch::async, *range_query, std::ref(trajectory)));
            }
            //if (auto knn_query = std::dynamic_pointer_cast<spatial_queries::KNN_Query_Test>(query_object)) {
                //knn_futures.emplace_back(std::async(std::launch::async, *knn_query, std::ref(trajectory)));
            //}
        }

        int correct_range_queries = process_futures(range_futures);
        int correct_knn_queries = process_futures(knn_futures);

        auto range_query_accuracy = static_cast<double>(correct_range_queries) / static_cast<double>(range_futures.size());
        auto knn_accuracy = static_cast<double>(correct_knn_queries) / static_cast<double>(knn_futures.size());
        return std::midpoint(range_query_accuracy, knn_accuracy);
    }

    TRACE_Q::MBR TRACE_Q::calculate_MBR(data_structures::Trajectory const& trajectory) {
        using data_structures::Location;
        TRACE_Q::MBR result{};
        result.t_low = trajectory.locations.front().timestamp;
        result.t_high = trajectory.locations.back().timestamp;

        auto [x_min, x_max] = std::ranges::minmax(trajectory.locations, [](Location const& loc1,
                Location const& loc2) { return loc1.longitude < loc2.longitude; });
        result.x_low = x_min.longitude;
        result.x_high = x_max.longitude;

        auto [y_min, y_max] = std::ranges::minmax(trajectory.locations, [](Location const& loc1,
                Location const& loc2) { return loc1.latitude < loc2.latitude; });
        result.y_low = y_min.latitude;
        result.y_high = y_max.latitude;

        return result;
    }

    TRACE_Q::MBR TRACE_Q::expand_MBR(MBR mbr) const {
        mbr.x_low = mbr.x_low / (1 + grid_expansion_factor);
        mbr.x_high = mbr.x_high * (1 + grid_expansion_factor);
        mbr.y_low = mbr.y_low / (1 + grid_expansion_factor);
        mbr.y_high = mbr.y_high * (1 + grid_expansion_factor);
        return mbr;
    }

    std::vector<std::shared_ptr<spatial_queries::Range_Query_Test>> TRACE_Q::range_query_initialization(
            data_structures::Trajectory const& trajectory, double x, double y, long double t, MBR const& mbr) const {
        std::vector<std::shared_ptr<spatial_queries::Range_Query_Test>> result{};
        std::vector<std::future<spatial_queries::Range_Query_Test>> futures{};

        for (int window_number = 0; window_number < windows_per_grid_point; ++window_number) {
            auto [window_x_low, window_x_high] = calculate_window_range(
                    x, mbr.x_low, mbr.x_high, window_expansion_rate, grid_density, window_number);
            auto [window_y_low, window_y_high] = calculate_window_range(
                    y, mbr.y_low, mbr.y_high, window_expansion_rate, grid_density, window_number);
            auto [window_t_low, window_t_high] = calculate_window_range(
                    t, mbr.t_low, mbr.t_high, window_expansion_rate,
                    time_interval_multiplier, window_number);

            futures.emplace_back(
                    std::async(std::launch::async,
                               [](const data_structures::Trajectory& original_trajectory, double x_low,
                                       double x_high, double y_low, double y_high, unsigned long t_low,
                                       unsigned long t_high)
                                       { return spatial_queries::Range_Query_Test{original_trajectory, x_low, x_high,
                                                                                  y_low, y_high, t_low, t_high};
                                           },
                               trajectory, window_x_low, window_x_high, window_y_low,
                               window_y_high, window_t_low, window_t_high));

        }

        for (auto& fut : futures) {
            result.push_back(std::make_shared<spatial_queries::Range_Query_Test>(fut.get()));
        }
        return result;
    }

    std::pair<long double, long double> TRACE_Q::calculate_window_range(
            long double center, long double mbr_low, long double mbr_high, double window_expansion_rate,
            double grid_density, int window_number) {
        long double w_low = center - 0.5 * (pow(window_expansion_rate, window_number)
                * grid_density * (mbr_high - mbr_low));
        if (w_low < mbr_low) {
            w_low = mbr_low;
        }
        long double w_high = center + 0.5 * (pow(window_expansion_rate, window_number)
                * grid_density * (mbr_high - mbr_low));
        if (w_high > mbr_high) {
            w_high = mbr_high;
        }
        return {w_low, w_high};
    }

    int TRACE_Q::process_futures(std::vector<std::future<bool>>& futures){
        int correct_queries = 0;
        for (auto & fut : futures) {
            if (fut.get()) {
                correct_queries++;
            }
        }
        return correct_queries;
    }

} // trace_q