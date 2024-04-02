#include <future>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "TRACE_Q.hpp"
#include "MRPA.hpp"

namespace trace_q {

    data_structures::Trajectory TRACE_Q::simplify(data_structures::Trajectory const& original_trajectory, double min_query_accuracy) const {
        auto simplifications = mrpa(original_trajectory);

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
                    auto t = mbr.t_low + t_point * time_interval_multiplier * (mbr.t_high - mbr.t_low);

                    auto range_queries = range_query_initialization(original_trajectory, x_coord, y_coord, t, mbr);
                    query_objects.insert(std::end(query_objects), std::begin(range_queries), std::end(range_queries));

                    // TODO: KNN
                }
            }
        }

        // iterate from the back since simplifications appear in decreasing resolution
        for (int i = static_cast<int>(simplifications.size()) - 1; i >= 0; --i) {
            if (query_accuracy(simplifications[i], query_objects) >= min_query_accuracy) {
                return simplifications[i];
            }
        }

        return original_trajectory;
    }


    double TRACE_Q::query_accuracy(data_structures::Trajectory const& trajectory,
                                   std::vector<std::shared_ptr<spatial_queries::Query>> const& query_objects) const {

        std::vector<std::future<bool>> futures{};
        for (auto const& query_object : query_objects) {
            if (auto range_query = std::dynamic_pointer_cast<spatial_queries::Range_Query>(query_object)) {
                futures.emplace_back(std::async(std::launch::async, *range_query, std::ref(trajectory)));
            }
            //if (auto knn_query = std::dynamic_pointer_cast<spatial_queries::KNN_Query>(query_object)) {
            //    futures.emplace_back(std::async(std::launch::async, *knn_query, std::ref(trajectory)));
            //}
        }

        int correct_queries = 0;
        for (auto & fut : futures) {
            if (fut.get()) {
                correct_queries++;
            }
        }

        return static_cast<double>(correct_queries) / query_amount;
    }

    int TRACE_Q::calculate_query_amount() const {
        auto points_in_net = static_cast<int>(std::pow(std::ceil(1.0 / grid_density) + 1, 2));

        auto range_queries = static_cast<int>(points_in_net * windows_per_grid_point
                                              * (std::ceil(1 / time_interval_multiplier) + 1));
        //auto knn_queries = static_cast<int>(points_in_net * (std::ceil(1 / time_interval_multiplier) + 1));

        return range_queries; //TODO: Add variable above
    }

    TRACE_Q::MBR TRACE_Q::calculate_MBR(data_structures::Trajectory const& trajectory) {
        using data_structures::Location;
        TRACE_Q::MBR result{};
        result.t_low = trajectory.locations.front().timestamp;
        result.t_high = trajectory.locations.back().timestamp;

        auto [x_min, x_max] = std::ranges::minmax(trajectory.locations, [](Location loc1, Location loc2) { return loc1.longitude < loc2.longitude; });
        result.x_low = x_min.longitude;
        result.x_high = x_max.longitude;

        auto [y_min, y_max] = std::ranges::minmax(trajectory.locations, [](Location loc1, Location loc2) { return loc1.latitude < loc2.latitude; });
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

    std::vector<std::shared_ptr<spatial_queries::Range_Query>> TRACE_Q::range_query_initialization(
            data_structures::Trajectory const& trajectory, double x, double y, long double t, MBR const& mbr) const {
        std::vector<std::shared_ptr<spatial_queries::Range_Query>> result{};
        std::vector<std::future<spatial_queries::Range_Query>> futures{};

        for (int window_number = 0; window_number < windows_per_grid_point; ++window_number) {
            auto w_x_low = x - 0.5 * (pow(window_expansion_rate, window_number)
                                      * grid_density * (mbr.x_high - mbr.x_low));
            if (w_x_low < mbr.x_low) {
                w_x_low = mbr.x_low;
            }
            auto w_x_high = x + 0.5 * (pow(window_expansion_rate, window_number)
                                       * grid_density * (mbr.x_high - mbr.x_low));
            if (w_x_high > mbr.x_high) {
                w_x_high = mbr.x_high;
            }

            auto w_y_low = y - 0.5 * (pow(window_expansion_rate, window_number)
                                      * grid_density * (mbr.y_high - mbr.y_low));
            if (w_y_low < mbr.y_low) {
                w_y_low = mbr.y_low;
            }
            auto w_y_high = y + 0.5 * (pow(window_expansion_rate, window_number)
                                       * grid_density * (mbr.y_high - mbr.y_low));
            if (w_y_high > mbr.y_high) {
                w_y_high = mbr.y_high;
            }

            auto w_t_low = t - 0.5 * (pow(window_expansion_rate, window_number)
                                      * time_interval_multiplier * (mbr.t_high - mbr.t_low));
            if (w_t_low < mbr.t_low) {
                w_t_low = mbr.t_low;
            }
            auto w_t_high = t + 0.5 * (pow(window_expansion_rate, window_number)
                                       * time_interval_multiplier * (mbr.t_high - mbr.t_low));
            if (w_t_high > mbr.t_high) {
                w_t_high = mbr.t_high;
            }

            futures.emplace_back(
                    std::async(std::launch::async,
                               [](const data_structures::Trajectory& original_trajectory, double x_low,
                                       double x_high, double y_low, double y_high, long double t_low,
                                       long double t_high)
                                       { return spatial_queries::Range_Query{original_trajectory, x_low, x_high,
                                                                             y_low, y_high, t_low, t_high};
                                           },
                                           trajectory, w_x_low, w_x_high, w_y_low,
                                           w_y_high, w_t_low, w_t_high));

        }

        for (auto& fut : futures) {
            result.push_back(std::make_shared<spatial_queries::Range_Query>(fut.get()));
        }
        return result;
    };

} // trace_q