#include "trajectory_data_handling/trajectory_sql.hpp"

#include "trajectory_data_handling/trajectory_file_manager.hpp"

int main() {
    Trajectory_File_Manager::File_Manager file_manager;
    Trajectory_SQL::Trajectory_Manager trajectory_manager;

//    trajectory_manager.reset_all_data();
//    trajectory_manager.create_database();
//    trajectory_manager.create_rtree_table();
//    file_manager.load_tdrive_dataset();
//    trajectory_manager.insert_trajectories_into_trajectory_table();
//    file_manager.load_geolife_dataset();
//    trajectory_manager.load_database_into_datastructure();
//    trajectory_manager.print_trajectories();
    trajectory_manager.load_trajectories_into_rtree();
//    trajectory_manager.reset_all_data();

    return 0;
}
