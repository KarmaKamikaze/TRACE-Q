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
                auto range_start = static_cast<int>(std::floor((static_cast<double>(traj.points.size() - 1)) / (resolution - 1) * (j - 1) + 1));
                auto range_end = static_cast<int>(std::floor((static_cast<double>(traj.points.size() - 1)) / (resolution - 1) * j + 1));

                error_tolerance += error_SED_sum(traj, range_start, range_end);
            }

            error_tolerance *= 1 / (resolution - 1);
            result.emplace_back(error_tolerance);
        }

        return result;
    }

    Trajectory MRPA::simplify_subtrajectory(Trajectory const& trajectory, int i, int j) {
        Trajectory res {};

        for (int k = i+1; k < j; ++k) {
            auto x = trajectory.points[i].x +
                    (trajectory.points[k].t - trajectory.points[i].t) /
                    (trajectory.points[j].t - trajectory.points[i].t) *
                    (trajectory.points[j].x - trajectory.points[i].x);
            auto y = trajectory.points[i].y +
                     (trajectory.points[k].t - trajectory.points[i].t) /
                     (trajectory.points[j].t - trajectory.points[i].t) *
                     (trajectory.points[j].y - trajectory.points[i].y);
            auto point = MRPA::Trajectory::Point(x,y,trajectory.points[k].t);
            res.points.emplace_back(point);
        }

        return res;
    }

    double MRPA::single_SED(Trajectory::Point const& original_point, Trajectory::Point const& approx_point)
    {
        return sqrt(pow(original_point.x - approx_point.x, 2) + pow(original_point.y - approx_point.y, 2));
    }

    double MRPA::error_SED_sum(const simp_algorithms::MRPA::Trajectory& trajectory, int i, int j)
    {
        auto approx_trajectory = simplify_subtrajectory(trajectory, i, j);
        double res {};
        for (int k = i+1; k < j; ++k) {
            res += pow(single_SED(trajectory.points[k], approx_trajectory.points[k]), 2);
        }

        return res;
    }


} // simp_algorithms