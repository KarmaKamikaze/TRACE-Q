#include "trajectory.h"
#include <iostream>
#include <string>
#include <cstdio>
#include <format>
#include "../../external/sqlite/sqlite3.h"
#include "sqlite_querying.h"

namespace data_structures{
    std::vector<Trajectory> allTrajectories;
}

void load_trajectories_into_rtree() {
    auto query = "SELECT trajectory_id, MIN(longitude) AS min_longitude, MAX(longitude) AS max_longitude,\n"
                        "MIN(latitude) AS min_latitude, MAX(latitude) AS max_latitude,\n"
                        "MIN(timestamp) AS min_timestamp, MAX(timestamp) AS max_timestamp\n"
                        "FROM trajectory_information\n"
                        "GROUP BY trajectory_id;";

    run_sql(query, insert_into_rtree_table);
}

void insert_trajectories_into_trajectory_table() {
    for (const auto & trajectory : data_structures::allTrajectories) {
        for (const auto &location : trajectory.locations) {
            char query[150];
            snprintf(query, sizeof(query), "INSERT INTO trajectory_information VALUES(NULL, %d, '%s', %f, %f)",
                     trajectory.id, location.timestamp.c_str(), location.longitude, location.latitude);

            run_sql(query, insert_into_trajectory_table);
        }
    }
}

void load_database_into_datastructure() {
    auto query = "SELECT trajectory_id, timestamp, longitude, latitude FROM trajectory_information";

    run_sql(query, load_trajectory_information_into_datastructure);
}

void print_trajectories() {
    for (const auto & trajectory : data_structures::allTrajectories) {
        std::cout << "id: " << trajectory.id << std::endl;
        for (const auto &location: trajectory.locations) {
            std::cout << "order: " << location.order << '\n';
            std::cout << "timestamp: " << location.timestamp << '\n';
            std::cout << "longitude, latitude: " << location.longitude << " " << location.latitude << '\n';
        }
    }
}

void create_database() {
    auto query = "CREATE TABLE trajectory_information(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, trajectory_id INTEGER NOT NULL, timestamp TEXT NOT NULL, longitude REAL NOT NULL, latitude REAL NOT NULL)";
    run_sql(query, create_table);
    query = "CREATE TABLE simplified_trajectory_information(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, trajectory_id INTEGER NOT NULL, timestamp TEXT NOT NULL, longitude REAL NOT NULL, latitude REAL NOT NULL)";
    run_sql(query, create_table);
}

void create_rtree_table() {
    auto query = "CREATE VIRTUAL TABLE trajectory_rtree USING rtree(id INTEGER PRIMARY KEY, minLongitude REAL, maxLongitude REAL, minLatitude REAL, maxLatitude REAL, minTimestamp TEXT, maxTimestamp TEXT)";
    run_sql(query, create_table);
    query = "CREATE VIRTUAL TABLE simplified_trajectory_rtree USING rtree(id INTEGER PRIMARY KEY, minLongitude REAL, maxLongitude REAL, minLatitude REAL, maxLatitude REAL, minTimestamp TEXT, maxTimestamp TEXT)";
    run_sql(query, create_table);
}

void reset_all_data() {
    run_sql("DROP TABLE trajectory_information", reset_database);
    run_sql("DROP TABLE simplified_trajectory_information", reset_database);

//    run_sql("DELETE FROM trajectory_information", reset_database);
//    run_sql("DELETE FROM trajectory_rtree", reset_database);
//    run_sql("DELETE FROM trajectory_rtree_rowid", reset_database);
//    run_sql("DELETE FROM trajectory_rtree_parent", reset_database);
//    run_sql("DELETE FROM trajectory_rtree_node", reset_database);
}

int returntwo() {
    return 2;
}





