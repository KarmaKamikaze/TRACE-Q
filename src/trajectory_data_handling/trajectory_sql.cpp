#include "trajectory_sql.hpp"
#include <iostream>
#include <string>
#include <cstdio>
#include <format>
#include "../../external/sqlite/sqlite3.h"

namespace data_structures{
    std::vector<Trajectory> allTrajectories;
}

namespace Trajectory_SQL {
    void Trajectory_Manager::load_trajectories_into_rtree() {
        auto query = "SELECT trajectory_id, MIN(longitude) AS min_longitude, MAX(longitude) AS max_longitude,\n"
                     "MIN(latitude) AS min_latitude, MAX(latitude) AS max_latitude,\n"
                     "MIN(timestamp) AS min_timestamp, MAX(timestamp) AS max_timestamp\n"
                     "FROM trajectory_information\n"
                     "GROUP BY trajectory_id;";

        query_handler.run_sql(query, SQLite_Querying::insert_into_rtree_table);
    }

    void Trajectory_Manager::insert_trajectories_into_trajectory_table() {
        for (const auto & trajectory : data_structures::allTrajectories) {
            for (const auto &location : trajectory.locations) {
                char query[150];
                snprintf(query, sizeof(query), "INSERT INTO trajectory_information VALUES(NULL, %d, '%s', %f, %f)",
                         trajectory.id, location.timestamp.c_str(), location.longitude, location.latitude);

                query_handler.run_sql(query, SQLite_Querying::insert_into_trajectory_table);
            }
        }
    }

    void Trajectory_Manager::load_database_into_datastructure() {
        auto query = "SELECT trajectory_id, timestamp, longitude, latitude FROM trajectory_information";

        query_handler.run_sql(query, SQLite_Querying::load_trajectory_information_into_datastructure);
    }

    void Trajectory_Manager::print_trajectories() {
        for (const auto & trajectory : data_structures::allTrajectories) {
            std::cout << "id: " << trajectory.id << std::endl;
            for (const auto &location: trajectory.locations) {
                std::cout << "m_order: " << location.order << '\n';
                std::cout << "timestamp: " << location.timestamp << '\n';
                std::cout << "longitude, latitude: " << location.longitude << " " << location.latitude << '\n';
            }
        }
    }

    void Trajectory_Manager::create_database() {
        auto query = "CREATE TABLE trajectory_information(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, trajectory_id INTEGER NOT NULL, timestamp TEXT NOT NULL, longitude REAL NOT NULL, latitude REAL NOT NULL)";
        query_handler.run_sql(query, SQLite_Querying::create_table);
        query = "CREATE TABLE simplified_trajectory_information(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, trajectory_id INTEGER NOT NULL, timestamp TEXT NOT NULL, longitude REAL NOT NULL, latitude REAL NOT NULL)";
        query_handler.run_sql(query, SQLite_Querying::create_table);
    }

    void Trajectory_Manager::Trajectory_Manager::create_rtree_table() {
        auto query = "CREATE VIRTUAL TABLE trajectory_rtree USING rtree(id INTEGER PRIMARY KEY, minLongitude REAL, maxLongitude REAL, minLatitude REAL, maxLatitude REAL, minTimestamp TEXT, maxTimestamp TEXT)";
        query_handler.run_sql(query, SQLite_Querying::create_table);
        query = "CREATE VIRTUAL TABLE simplified_trajectory_rtree USING rtree(id INTEGER PRIMARY KEY, minLongitude REAL, maxLongitude REAL, minLatitude REAL, maxLatitude REAL, minTimestamp TEXT, maxTimestamp TEXT)";
        query_handler.run_sql(query, SQLite_Querying::create_table);
    }

    void Trajectory_Manager::reset_all_data() {
        query_handler.run_sql("DELETE FROM trajectory_information", SQLite_Querying::reset_database);
        query_handler.run_sql("DELETE FROM trajectory_rtree", SQLite_Querying::reset_database);
        query_handler.run_sql("DELETE FROM trajectory_rtree_rowid", SQLite_Querying::reset_database);
        query_handler.run_sql("DELETE FROM trajectory_rtree_parent", SQLite_Querying::reset_database);
        query_handler.run_sql("DELETE FROM trajectory_rtree_node", SQLite_Querying::reset_database);
    }
}







