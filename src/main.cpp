#include <iostream>
#include "trajectory_data_handling/trajectory_sql.hpp"
#include "trajectory_data_handling/sqlite_querying.hpp"
#include "trajectory_data_handling/trajectory_file_manager.hpp"
#include "../src/simp-algorithms/MRPA.hpp"

int main() {
    trajectory_file_manager::file_manager file_manager{};
    auto all_trajectories = std::make_shared<std::vector<data_structures::Trajectory>>();
    sqlite_querying::query_handler::all_trajectories = all_trajectories;
    trajectory_sql::trajectory_manager trajectory_manager{};


//    trajectory_manager.reset_all_data();
    trajectory_manager.create_database();
//    trajectory_manager.create_rtree_table();
//    file_manager.load_tdrive_dataset(*alltrajectories);
    file_manager.load_geolife_dataset(*all_trajectories);
    trajectory_manager.insert_trajectories_into_trajectory_table(*all_trajectories);
//    trajectory_manager.load_database_into_datastructure();
//    trajectory_manager.print_trajectories(*alltrajectories);
//    trajectory_manager.load_trajectories_into_rtree();
//    trajectory_manager.reset_all_data();


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

    auto mrpa = simp_algorithms::MRPA{};
    auto results = mrpa(t);

    for (int i = 0; i < results.size(); i++) {
        std::cout << "Candidate " << i + 1 << "\n";
        for (const auto& point : results[i].points) {
            std::cout << "Order: " << point.order << ", x: " << point.x << ", y: " << point.y << ", t: " << point.t << "\n";
        }
    }

    return 0;
}
