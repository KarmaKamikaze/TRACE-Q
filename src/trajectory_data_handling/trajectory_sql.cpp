#include "trajectory_sql.hpp"
#include <iostream>
#include <cstdio>
#include "../../external/sqlite/sqlite3.h"

namespace trajectory_sql {
    void trajectory_manager::load_trajectories_into_rtree() {
        auto query = "SELECT trajectory_id, MIN(longitude) AS min_longitude, MAX(longitude) AS max_longitude,\n"
                     "MIN(latitude) AS min_latitude, MAX(latitude) AS max_latitude,\n"
                     "MIN(timestamp) AS min_timestamp, MAX(timestamp) AS max_timestamp\n"
                     "FROM trajectory_information\n"
                     "GROUP BY trajectory_id;";

        sqlite_querying::query_handler::run_sql(query, sqlite_querying::insert_into_rtree_table);
    }

    void trajectory_manager::insert_trajectories_into_trajectory_table(std::vector<data_structures::Trajectory> &all_trajectories) {
        for (const auto & trajectory : all_trajectories) {
            for (const auto &location : trajectory.locations) {
                std::stringstream query{};
                query << "INSERT INTO trajectory_information VALUES(NULL, " << trajectory.id << ", '"
                    << location.timestamp << "', " << location.longitude << ", " << location.latitude << ")";

                sqlite_querying::query_handler::run_sql(query.str().c_str(), sqlite_querying::insert_into_trajectory_table);
            }
        }
    }

    void trajectory_manager::load_database_into_datastructure() {
        auto query = "SELECT trajectory_id, timestamp, longitude, latitude FROM trajectory_information";

        sqlite_querying::query_handler::run_sql(query, sqlite_querying::load_trajectory_information_into_datastructure);
    }

    void trajectory_manager::print_trajectories(std::vector<data_structures::Trajectory> &all_trajectories) {
        for (const auto & trajectory : all_trajectories) {
            std::cout << "id: " << trajectory.id << std::endl;
            for (const auto &location: trajectory.locations) {
                std::cout << "m_order: " << location.order << '\n';
                std::cout << "timestamp: " << std::fixed <<  location.timestamp << '\n';
                std::cout << "longitude, latitude: " << location.longitude << " " << location.latitude << '\n';
            }
        }
    }

    void trajectory_manager::create_database() {
        auto query = "CREATE TABLE trajectory_information(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, trajectory_id INTEGER NOT NULL, timestamp REAL NOT NULL, longitude REAL NOT NULL, latitude REAL NOT NULL)";
        sqlite_querying::query_handler::run_sql(query, sqlite_querying::create_table);
        query = "CREATE TABLE simplified_trajectory_information(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, trajectory_id INTEGER NOT NULL, timestamp REAL NOT NULL, longitude REAL NOT NULL, latitude REAL NOT NULL)";
        sqlite_querying::query_handler::run_sql(query, sqlite_querying::create_table);
    }

    void trajectory_manager::create_rtree_table() {
        auto query = "CREATE VIRTUAL TABLE trajectory_rtree USING rtree(id INTEGER PRIMARY KEY, minLongitude REAL, maxLongitude REAL, minLatitude REAL, maxLatitude REAL, minTimestamp TEXT, maxTimestamp TEXT)";
        sqlite_querying::query_handler::run_sql(query, sqlite_querying::create_table);
        query = "CREATE VIRTUAL TABLE simplified_trajectory_rtree USING rtree(id INTEGER PRIMARY KEY, minLongitude REAL, maxLongitude REAL, minLatitude REAL, maxLatitude REAL, minTimestamp TEXT, maxTimestamp TEXT)";
        sqlite_querying::query_handler::run_sql(query, sqlite_querying::create_table);
    }

    void trajectory_manager::reset_all_data() {
        sqlite_querying::query_handler::run_sql("DROP TABLE trajectory_information", sqlite_querying::reset_database);
        sqlite_querying::query_handler::run_sql("DROP TABLE simplified_trajectory_information", sqlite_querying::reset_database);
        sqlite_querying::query_handler::run_sql("DROP TABLE trajectory_rtree", sqlite_querying::reset_database);
        sqlite_querying::query_handler::run_sql("DROP TABLE trajectory_rtree_rowid", sqlite_querying::reset_database);
        sqlite_querying::query_handler::run_sql("DROP TABLE trajectory_rtree_parent", sqlite_querying::reset_database);
        sqlite_querying::query_handler::run_sql("DROP TABLE trajectory_rtree_node", sqlite_querying::reset_database);
    }
}







