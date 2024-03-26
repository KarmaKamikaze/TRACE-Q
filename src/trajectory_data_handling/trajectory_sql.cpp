#include "trajectory_sql.hpp"
#include <iostream>

namespace trajectory_data_handling {
    void trajectory_manager::load_trajectories_into_original_rtree() {
        auto query = std::string{"SELECT trajectory_id, MIN(longitude) AS min_longitude, MAX(longitude) AS max_longitude,\n"
                     "MIN(latitude) AS min_latitude, MAX(latitude) AS max_latitude,\n"
                     "MIN(timestamp) AS min_timestamp, MAX(timestamp) AS max_timestamp\n"
                     "FROM trajectory_information\n"
                     "GROUP BY trajectory_id;"};

        trajectory_data_handling::query_handler::run_sql(query, trajectory_data_handling::insert_into_original_rtree_table);
    }

    void trajectory_manager::load_trajectories_into_simplified_rtree() {
        auto query = std::string{"SELECT trajectory_id, MIN(longitude) AS min_longitude, MAX(longitude) AS max_longitude,\n"
                                 "MIN(latitude) AS min_latitude, MAX(latitude) AS max_latitude,\n"
                                 "MIN(timestamp) AS min_timestamp, MAX(timestamp) AS max_timestamp\n"
                                 "FROM simplified_trajectory_information\n"
                                 "GROUP BY trajectory_id;"};

        trajectory_data_handling::query_handler::run_sql(query, trajectory_data_handling::insert_into_simplified_rtree_table);
    }

    void trajectory_manager::insert_trajectories_into_trajectory_table(std::vector<data_structures::Trajectory> &all_trajectories, db_table table) {
        std::string table_name{};
        switch(table) {
            case original_trajectories:
                table_name = "trajectory_information";
                break;
            case simplified_trajectories:
                table_name = "simplified_trajectory_information";
                break;
        }

        for (const auto & trajectory : all_trajectories) {
            for (const auto &location : trajectory.locations) {
                std::stringstream query{};

                query << "INSERT INTO " << table_name << " VALUES(NULL,"  << trajectory.id << ", '"
                    << location.timestamp << "', " << location.longitude << ", " << location.latitude << ")";

                trajectory_data_handling::query_handler::run_sql(query.str().c_str(), trajectory_data_handling::insert_into_trajectory_table);
            }
        }
    }

    void trajectory_manager::load_database_into_datastructure(db_table table) {
        std::string table_name{};
        std::stringstream query{};
        switch(table) {
            case original_trajectories:
                table_name = "trajectory_information";
                query << "SELECT trajectory_id, timestamp, longitude, latitude FROM " << table_name;
                trajectory_data_handling::query_handler::run_sql(query.str().c_str(), trajectory_data_handling::load_original_trajectory_information_into_datastructure);
                break;
            case simplified_trajectories:
                table_name = "simplified_trajectory_information";
                query << "SELECT trajectory_id, timestamp, longitude, latitude FROM " << table_name;
                trajectory_data_handling::query_handler::run_sql(query.str().c_str(), trajectory_data_handling::load_simplified_trajectory_information_into_datastructure);
                break;
            default:
                std::cout << "Error in switch statement in load_database_into_datastructure." << '\n';
        }

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
        auto query = std::string{"CREATE TABLE trajectory_information(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, trajectory_id INTEGER NOT NULL, timestamp REAL NOT NULL, longitude REAL NOT NULL, latitude REAL NOT NULL)"};
        trajectory_data_handling::query_handler::run_sql(query, trajectory_data_handling::create_table);
        query = std::string{"CREATE TABLE simplified_trajectory_information(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, trajectory_id INTEGER NOT NULL, timestamp REAL NOT NULL, longitude REAL NOT NULL, latitude REAL NOT NULL)"};
        trajectory_data_handling::query_handler::run_sql(query, trajectory_data_handling::create_table);
    }

    void trajectory_manager::create_rtree_table() {
        auto query = std::string{"CREATE VIRTUAL TABLE trajectory_rtree USING rtree(id INTEGER PRIMARY KEY, minLongitude REAL, maxLongitude REAL, minLatitude REAL, maxLatitude REAL, minTimestamp TEXT, maxTimestamp TEXT)"};
        trajectory_data_handling::query_handler::run_sql(query, trajectory_data_handling::create_table);
        query = std::string{"CREATE VIRTUAL TABLE simplified_trajectory_rtree USING rtree(id INTEGER PRIMARY KEY, minLongitude REAL, maxLongitude REAL, minLatitude REAL, maxLatitude REAL, minTimestamp TEXT, maxTimestamp TEXT)"};
        trajectory_data_handling::query_handler::run_sql(query, trajectory_data_handling::create_table);
    }

    void trajectory_manager::reset_all_data() {
        trajectory_data_handling::query_handler::run_sql("DROP TABLE trajectory_information", trajectory_data_handling::reset_database);
        trajectory_data_handling::query_handler::run_sql("DROP TABLE simplified_trajectory_information", trajectory_data_handling::reset_database);
        trajectory_data_handling::query_handler::run_sql("DROP TABLE trajectory_rtree", trajectory_data_handling::reset_database);
        trajectory_data_handling::query_handler::run_sql("DROP TABLE trajectory_rtree_rowid", trajectory_data_handling::reset_database);
        trajectory_data_handling::query_handler::run_sql("DROP TABLE trajectory_rtree_parent", trajectory_data_handling::reset_database);
        trajectory_data_handling::query_handler::run_sql("DROP TABLE trajectory_rtree_node", trajectory_data_handling::reset_database);
    }
}







