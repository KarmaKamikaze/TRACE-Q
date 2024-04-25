#include <cmath>
#include "KNN_Query_Test.hpp"

namespace spatial_queries {
    std::string KNN_Query_Test::table_name{"original_trajectories"};

    bool KNN_Query_Test::operator()(data_structures::Trajectory const& trajectory) {

        auto min_distance_simplified = std::numeric_limits<double>::max();

        for (const auto& location : trajectory.locations) {
            auto location_distance = euclidean_distance(location, origin);
            if (location_distance < min_distance_simplified && location.timestamp >= origin.t_low
            && location.timestamp <= origin.t_high) {
                min_distance_simplified = location_distance;
            }
        }

        if (min_distance_simplified <= query_result.back().distance) {
            return true;
        }

        return false;
    }

    double KNN_Query_Test::euclidean_distance(data_structures::Location const& location, KNN_Query::KNN_Origin const& origin) {
        return std::sqrt(std::pow(location.longitude - origin.x, 2) + std::pow(location.latitude - origin.y, 2));
    }

} // spatial_queries