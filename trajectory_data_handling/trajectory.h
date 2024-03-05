#ifndef P8_PROJECT_TRAJECTORY_H
#define P8_PROJECT_TRAJECTORY_H

#include <vector>

#include "../datastructure/trajectory_structure.h"

void load_trajectories_into_rtree();
void insert_trajectories_into_trajectory_table();
void load_database_into_datastructure();
void print_trajectories();
void create_database();
void create_rtree_table();
void reset_all_data();

#endif //P8_PROJECT_TRAJECTORY_H
