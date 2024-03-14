#include "trajectory_data_handling/trajectory_sql.hpp"
#include "trajectory_data_handling/sqlite_querying.hpp"
#include "trajectory_data_handling/trajectory_file_manager.hpp"

int main() {
    trajectory_file_manager::file_manager file_manager{};
    std::shared_ptr<std::vector<data_structures::Trajectory>> alltrajectories = std::make_shared<std::vector<data_structures::Trajectory>>();
    sqlite_querying::query_handler::all_trajectories = alltrajectories;
    trajectory_sql::trajectory_manager trajectory_manager{};


//    trajectory_manager.reset_all_data();
//    trajectory_manager.create_database();
//    trajectory_manager.create_rtree_table();
//    file_manager.load_tdrive_dataset(*alltrajectories);
//    trajectory_manager.insert_trajectories_into_trajectory_table(*alltrajectories);
//    file_manager.load_geolife_dataset(*all_trajectories);
//    trajectory_manager.load_database_into_datastructure();
//    trajectory_manager.print_trajectories(*alltrajectories);
//    trajectory_manager.load_trajectories_into_rtree();
//    trajectory_manager.reset_all_data();

    return 0;
}
