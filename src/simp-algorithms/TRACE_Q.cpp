#include <future>
#include <iostream>
#include "TRACE_Q.hpp"
#include "MRPA.hpp"
#include "../querying/Range_Query.hpp"
#include "../querying/KNN_Query.hpp"

namespace trace_q {

    data_structures::Trajectory TRACE_Q::simplify(data_structures::Trajectory const& original_trajectory, double max_locations, double min_query_accuracy) const {
        auto mrpa = simp_algorithms::MRPA(2);
        auto simplifications = mrpa(original_trajectory);

        std::vector<std::shared_ptr<spatial_queries::Query>> query_objects{};
        for (int i = 0; i < query_amount; ++i) {
            query_objects.push_back(std::make_shared<spatial_queries::Range_Query>(original_trajectory, 0, 350, 0, 250, 1, 100));
        }

        for (const auto& simp_traj : simplifications) {
            std::cout << query_error(simp_traj, query_objects);
        }

        return simplifications.front();
    }


    double TRACE_Q::query_error(data_structures::Trajectory const& trajectory,
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
        auto points_in_net = static_cast<int>(std::pow(std::round(1.0 / grid_density_multiplier) + 1, 2));

        auto range_queries = static_cast<int>(points_in_net * windows_per_grid_point
                                              * std::round(1 / time_interval_multiplier));
        auto knn_queries = static_cast<int>(points_in_net * std::round(1 / time_interval_multiplier));

        return range_queries + knn_queries;
    }

} // trace_q