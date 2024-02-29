#include <iostream>
#include "Simp-Algorithms/MRPA.h"

int main() {
    simp_algorithms::Trajectory t {};
    t.points.emplace_back(simp_algorithms::Trajectory::Point(1, 2, 0));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(4, 2, 1));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(7, 4, 4));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(17, 9, 12));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(26, 12, 15));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(32, 5, 18));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(69, 3, 22));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(110, 26, 23));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(112, 23, 24));

    simp_algorithms::MRPA mrpa{};

    //for (auto simp_traj = mrpa.simplify_subtrajectory(t, 0, 3); const auto& traj : simp_traj.points) {
    //    std::cout << "x: " << traj.x << ", y: " << traj.y << ", t: " << traj.t << "\n";
    //}

    //std::cout << mrpa.error_SED_sum(t, 0, 3) << "\n";

    for (auto err_tol = mrpa.error_tolerance_init(t); const auto& err : err_tol) {
        std::cout << err << " ";
    }
    std::cout << "\n";

    return 0;
}
