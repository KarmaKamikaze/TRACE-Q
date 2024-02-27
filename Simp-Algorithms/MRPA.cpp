//
// Created by karma on 2/22/24.
//

#include <cmath>
#include "MRPA.h"

namespace simp_algorithms {
    std::vector<double> MRPA::error_tolerance_init(Trajectory const& traj) {
        std::vector<double> result{};

        auto number_of_tolerances = static_cast<int>(std::log(traj.points.size()) / std::log(resolution_scale));

        for (auto k = 0; k < number_of_tolerances; k++) {
            auto resolution = static_cast<double>(traj.points.size()) / std::pow(resolution_scale, k);
            double error_tolerance{};

            for (auto j = 1; j < resolution; j++) {
                error_tolerance += LSSD();
            }

            error_tolerance *= 1 / (resolution - 1);
            result.emplace_back(error_tolerance);
        }

        return result;
    }

    Trajectory MRPA::approximate_simplified_trajectory(Trajectory const& original_trajectory) {

    }

} // simp_algorithms