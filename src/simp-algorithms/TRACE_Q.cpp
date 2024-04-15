#include <future>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <limits>
#include "TRACE_Q.hpp"
#include "MRPA.hpp"

namespace trace_q {

    data_structures::Trajectory TRACE_Q::simplify(data_structures::Trajectory const& original_trajectory, double min_query_accuracy) const {
        auto simplifications = mrpa(original_trajectory);

        auto query_objects = initialize_query_tests(original_trajectory);

        // iterate from the back since simplifications appear in decreasing resolution
        for (int i = static_cast<int>(simplifications.size()) - 1; i >= 0; --i) {
            if (query_accuracy(simplifications[i], query_objects) >= min_query_accuracy) {
                return simplifications[i];
            }
        }
        return original_trajectory;
    }

    std::vector<std::shared_ptr<spatial_queries::Query>> TRACE_Q::initialize_query_tests(
            data_structures::Trajectory const& original_trajectory) const {
        std::vector<std::shared_ptr<spatial_queries::Query>> query_objects{};

        auto range_query_mbr = expand_MBR(calculate_MBR(original_trajectory), range_query_grid_expansion_factor);

        // Defines how many points there should be on both the x and y axes on the grid in range queries.
        auto range_query_points_on_axis = static_cast<int>(std::ceil(1 / range_query_grid_density));
        // Defines how many time intervals there should be made for each point in the grid in range queries.
        auto range_query_time_points = static_cast<int>(std::ceil(1 / range_query_time_interval_multiplier));

        for (int x_point = 0; x_point <= range_query_points_on_axis; ++x_point) {
            // Scale the coordinate based on the grid density, the mbr and the current point on the x-axis.
            auto x_coord = range_query_mbr.x_low + x_point * range_query_grid_density * (range_query_mbr.x_high - range_query_mbr.x_low);
            for (int y_point = 0; y_point <= range_query_points_on_axis; ++y_point) {
                auto y_coord = range_query_mbr.y_low + y_point * range_query_grid_density * (range_query_mbr.y_high - range_query_mbr.y_low);
                for (int t_point = 0; t_point <= range_query_time_points; ++t_point) {
                    auto t_interval = static_cast<long double>(range_query_mbr.t_low) + t_point * range_query_time_interval_multiplier
                                                                                        * static_cast<long double>((range_query_mbr.t_high - range_query_mbr.t_low));

                    auto range_queries = range_query_initialization(original_trajectory,
                                                                    x_coord, y_coord, t_interval, range_query_mbr);
                    query_objects.insert(std::end(query_objects), std::begin(range_queries),
                                         std::end(range_queries));
                }
            }
        }

        auto knn_query_mbr = expand_MBR(calculate_MBR(original_trajectory), knn_query_grid_expansion_factor);

        auto knn_query_points_on_axis = static_cast<int>(std::ceil(1 / knn_query_grid_density));
        auto knn_query_time_points = static_cast<int>(std::ceil(1 / knn_query_time_interval_multiplier));

        std::vector<std::future<std::shared_ptr<spatial_queries::KNN_Query_Test>>> knn_futures{};

        for (int x_point = 0; x_point <= knn_query_points_on_axis; ++x_point) {
            // Scale the coordinate based on the grid density, the mbr and the current point on the x-axis.
            auto x_coord = knn_query_mbr.x_low + x_point * knn_query_grid_density * (knn_query_mbr.x_high - knn_query_mbr.x_low);
            for (int y_point = 0; y_point <= knn_query_points_on_axis; ++y_point) {
                auto y_coord = knn_query_mbr.y_low + y_point * knn_query_grid_density * (knn_query_mbr.y_high - knn_query_mbr.y_low);
                for (int t_point = 0; t_point <= knn_query_time_points; ++t_point) {
                    auto t_interval = static_cast<long double>(knn_query_mbr.t_low) + t_point * knn_query_time_interval_multiplier
                                                                                        * static_cast<long double>((knn_query_mbr.t_high - knn_query_mbr.t_low));

                    knn_futures.emplace_back(std::async(std::launch::async, knn_query_initialization, x_coord, y_coord, t_interval, knn_query_mbr));
                }

                spatial_queries::KNN_Query::KNN_Origin origin{x_coord, y_coord, std::numeric_limits<long double>::min(), std::numeric_limits<long double>::max()};

                knn_futures.emplace_back(
                        std::async(std::launch::async,
                                   [this](spatial_queries::KNN_Query::KNN_Origin const& o)
                                   { return std::make_shared<spatial_queries::KNN_Query_Test>(knn_k, o); }, origin));

            }
        }

        for (auto& fut : knn_futures) {
            query_objects.push_back(fut.get());
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
            if (auto knn_query = std::dynamic_pointer_cast<spatial_queries::KNN_Query_Test>(query_object)) {
                knn_futures.emplace_back(std::async(std::launch::async, *knn_query, std::ref(trajectory)));
            }
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

    TRACE_Q::MBR TRACE_Q::expand_MBR(MBR mbr, double expansion_factor) {
        mbr.x_low = mbr.x_low / (1 + expansion_factor);
        mbr.x_high = mbr.x_high * (1 + expansion_factor);
        mbr.y_low = mbr.y_low / (1 + expansion_factor);
        mbr.y_high = mbr.y_high * (1 + expansion_factor);
        return mbr;
    }

    std::vector<std::shared_ptr<spatial_queries::Range_Query_Test>> TRACE_Q::range_query_initialization(
            data_structures::Trajectory const& trajectory, double x, double y, long double t, MBR const& mbr) const {
        std::vector<std::shared_ptr<spatial_queries::Range_Query_Test>> result{};
        std::vector<std::future<spatial_queries::Range_Query_Test>> futures{};

        for (int window_number = 0; window_number < windows_per_grid_point; ++window_number) {
            auto [window_x_low, window_x_high] = calculate_window_range(
                    x, mbr.x_low, mbr.x_high, window_expansion_rate, range_query_grid_density, window_number);
            auto [window_y_low, window_y_high] = calculate_window_range(
                    y, mbr.y_low, mbr.y_high, window_expansion_rate, range_query_grid_density, window_number);
            auto [window_t_low, window_t_high] = calculate_window_range(
                    t, mbr.t_low, mbr.t_high, window_expansion_rate,
                    range_query_time_interval_multiplier, window_number);

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

    std::shared_ptr<spatial_queries::KNN_Query_Test> TRACE_Q::knn_query_initialization(
            double x, double y, long double t, MBR const& mbr) {

        auto [t_low, t_high] = calculate_window_range(
                t, mbr.t_low, mbr.t_high, window_expansion_rate,
                knn_query_time_interval_multiplier, 0);

        spatial_queries::KNN_Query::KNN_Origin origin{x, y, t_low, t_high};
        return std::make_shared<spatial_queries::KNN_Query_Test>(knn_k, origin);
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