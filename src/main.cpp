#include <iostream>
#include "../src/simp-algorithms/MRPA.hpp"

int main() {
    simp_algorithms::Trajectory t {};
    t.points.emplace_back(simp_algorithms::Trajectory::Point(1, 1, 2, 0));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(2, 4, 2, 1));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(3, 7, 4, 4));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(4, 17, 9, 12));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(5,26, 12, 15));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(6, 32, 5, 18));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(7,69, 3, 22));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(8, 110, 21, 23));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(9, 112, 23, 24));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(10, 143, 28, 28));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(11, 145, 31, 30));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(12, 156, 56, 35));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(13, 169, 61, 37));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(14,173, 66, 41));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(15, 182, 69, 44));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(16,243, 98, 50));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(17, 277, 143, 53));
    t.points.emplace_back(simp_algorithms::Trajectory::Point(18, 300, 200, 60));

    simp_algorithms::MRPA mrpa{};

    for (auto err_tol = mrpa.error_tolerance_init(t); const auto& err : err_tol) {
        std::cout << err << " ";
    }
    std::cout << "\n";
    
    auto node = mrpa.init_tree(t, 600.0, 1000.0);

    return 0;
}
