#ifndef P8_PROJECT_TRAJECTORY_H
#define P8_PROJECT_TRAJECTORY_H

#include <vector>

#include "../data/trajectory_structure.hpp"
#include "sqlite_querying.hpp"

namespace trajectory_data_handling {
    class trajectory_manager {
    public:
        void load_trajectories_into_rtree();
        void insert_trajectories_into_trajectory_table(std::vector<data_structures::Trajectory> &all_trajectories);
        void load_database_into_datastructure();
        void print_trajectories(std::vector<data_structures::Trajectory> &all_trajectories);
        void create_database();
        void create_rtree_table();
        void reset_all_data();
    };
}

#endif
