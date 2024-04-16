#include <cmath>
#include "KNN_Query_Test.hpp"

namespace spatial_queries {
    std::string KNN_Query_Test::table_name{"original_trajectories"};

    bool KNN_Query_Test::operator()(data_structures::Trajectory const& trajectory) {

        // If there are no nearest neighbour results due to time constraints,
        // the simplification cannot possibly appear within the results either.
        if (query_result.empty()) {
            return true;
        }

        auto min_distance_simplified = std::numeric_limits<double>::max();

        for (const auto& location : trajectory.locations) {
            auto location_distance = euclidean_distance(location, origin);
            if (location_distance < min_distance_simplified && location.timestamp >= origin.t_low
            && location.timestamp <= origin.t_high) {
                min_distance_simplified = location_distance;
            }
        }

        for (int i = 0; i < query_result.size(); ++i) {
            if (query_result[i].id == trajectory.id) {
                if (i > 0 && i < query_result.size() - 1) {
                    return min_distance_simplified >= query_result[i - 1].distance
                    && min_distance_simplified <= query_result[i + 1].distance;
                }
                // Handle cases where trajectory is at the start or end of the vector
                else if (i == 0) {
                    return min_distance_simplified <= query_result[i + 1].distance;
                }
                else {  // i == query_result.size() - 1
                    return min_distance_simplified >= query_result[i - 1].distance;
                }
            }
        }

        // If the trajectory is not part of the K-Nearest-Neighbours, return true
        return true;
    }

    double KNN_Query_Test::euclidean_distance(data_structures::Location const& location, KNN_Query::KNN_Origin const& origin) {
        return std::sqrt(std::pow(location.longitude - origin.x, 2) + std::pow(location.latitude - origin.y, 2));
    }

} // spatial_queries