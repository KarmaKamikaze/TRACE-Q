#include "trajectory_sql.hpp"
#include <iostream>

namespace trajectory_data_handling {
    void trajectory_manager::load_trajectories_into_rtree(query_purpose purpose) {
        std::string table_name{};

        switch(purpose) {
            case query_purpose::insert_into_original_rtree_table:
                table_name = "trajectory_information";
                break;
            case query_purpose::insert_into_simplified_rtree_table:
                table_name = "simplified_trajectory_information";
                break;
        }

        auto query = std::string{"SELECT trajectory_id, MIN(longitude) AS min_longitude, MAX(longitude) AS max_longitude,\n"
                "MIN(latitude) AS min_latitude, MAX(latitude) AS max_latitude,\n"
                "MIN(timestamp) AS min_timestamp, MAX(timestamp) AS max_timestamp\n"
                "FROM " + table_name + "\n"
                "GROUP BY trajectory_id;"};

        trajectory_data_handling::query_handler::run_sql(query, purpose);
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

                trajectory_data_handling::query_handler::run_sql(query.str().c_str(), query_purpose::insert_into_trajectory_table);
            }
        }
    }

    void trajectory_manager::load_database_into_datastructure(query_purpose purpose, std::vector<std::string> const& id = {}) {
        std::string table_name{};
        std::stringstream query{};

        switch(purpose) {
            case query_purpose::load_original_trajectory_information_into_datastructure:
                table_name = "trajectory_information";
                break;
            case query_purpose::load_simplified_trajectory_information_into_datastructure:
                table_name = "simplified_trajectory_information";
                break;

            default:
                std::cout << "Error in switch statement in load_database_into_datastructure." << '\n';
        }

        if(id.empty()) {
            query << "SELECT trajectory_id, timestamp, longitude, latitude FROM " << table_name << ";";
        }
        else {
            query << "SELECT trajectory_id, timestamp, longitude, latitude FROM " << table_name << " WHERE trajectory_id IN (";
            for(auto i = id.begin(); i != id.end(); ++i) {
                query << *i;
                if (std::next(i) != id.end()) {
                    query << ",";
                }
            }
            query << ");";
        }
        trajectory_data_handling::query_handler::run_sql(query.str().c_str(), purpose);
    }

    void trajectory_manager::spatial_range_query_on_rtree_table(query_purpose purpose, std::tuple<float, float> longitudeRange, std::tuple<float, float> latitudeRange, std::tuple<float, float> timestampRange) {
        std::string table_name{};
        std::stringstream query{};
        int index{};
        auto original_trajectories = std::make_shared<std::vector<data_structures::Trajectory>>();
        trajectory_data_handling::query_handler::original_trajectories = original_trajectories;

        auto simplified_trajectories = std::make_shared<std::vector<data_structures::Trajectory>>();
        trajectory_data_handling::query_handler::simplified_trajectories = simplified_trajectories;

        spatial_queries::Range_Query::Window window{get<0>(longitudeRange), get<1>(longitudeRange), get<0>(latitudeRange), get<1>(latitudeRange), get<0>(timestampRange), get<1>(timestampRange)};

        switch(purpose) {
            case query_purpose::load_original_rtree_into_datastructure:
                table_name = "trajectory_rtree";
                break;
            case query_purpose::load_simplified_rtree_into_datastructure:
                table_name = "simplified_trajectory_rtree";
                break;

            default:
                std::cout << "Error in switch statement in spatial_query_on_rtree_table." << '\n';
        }

        query << "SELECT id FROM " << table_name << " WHERE minLongitude<=" << get<1>(longitudeRange) << " AND maxLongitude>=" << get<0>(longitudeRange) << " AND minLatitude<=" << get<1>(latitudeRange) << " AND maxLatitude>=" << get<0>(latitudeRange) << " AND minTimestamp<=" << get<1>(timestampRange) << " AND maxTimestamp>=" << get<0>(timestampRange) << ";";
        trajectory_data_handling::query_handler::run_sql(query.str().c_str(), purpose);

        for(const auto &id : trajectory_data_handling::query_handler::trajectory_ids_in_range) {
            std::cout << id << std::endl;
        }

        if (!trajectory_data_handling::query_handler::trajectory_ids_in_range.empty()){
            switch(purpose) {
                case query_purpose::load_original_rtree_into_datastructure:
                    load_database_into_datastructure(query_purpose::load_original_trajectory_information_into_datastructure,trajectory_data_handling::query_handler::trajectory_ids_in_range);
                    for(const auto &trajectory : *original_trajectories) {
                        index + 1;
                        auto keep_trajectory = spatial_queries::Range_Query::in_range(trajectory, window);
                        if (!keep_trajectory){
                            original_trajectories->erase(original_trajectories->begin() + index);
                        }
                    }
                    break;
                case query_purpose::load_simplified_rtree_into_datastructure:
                    load_database_into_datastructure(query_purpose::load_simplified_trajectory_information_into_datastructure,  trajectory_data_handling::query_handler::trajectory_ids_in_range);
                    for(const auto &trajectory : *original_trajectories) {
                        index + 1;
                        auto keep_trajectory = spatial_queries::Range_Query::in_range(trajectory, window);
                        if (!keep_trajectory){
                            original_trajectories->erase(original_trajectories->begin() + index);
                        }
                    }
                    break;
            }
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
        trajectory_data_handling::query_handler::run_sql(query, query_purpose::create_table);
        query = std::string{"CREATE TABLE simplified_trajectory_information(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, trajectory_id INTEGER NOT NULL, timestamp REAL NOT NULL, longitude REAL NOT NULL, latitude REAL NOT NULL)"};
        trajectory_data_handling::query_handler::run_sql(query, query_purpose::create_table);
    }

    void trajectory_manager::create_rtree_table() {
        auto query = std::string{"CREATE VIRTUAL TABLE trajectory_rtree USING rtree(id INTEGER PRIMARY KEY, minLongitude REAL, maxLongitude REAL, minLatitude REAL, maxLatitude REAL, minTimestamp TEXT, maxTimestamp TEXT)"};
        trajectory_data_handling::query_handler::run_sql(query, query_purpose::create_table);
        query = std::string{"CREATE VIRTUAL TABLE simplified_trajectory_rtree USING rtree(id INTEGER PRIMARY KEY, minLongitude REAL, maxLongitude REAL, minLatitude REAL, maxLatitude REAL, minTimestamp TEXT, maxTimestamp TEXT)"};
        trajectory_data_handling::query_handler::run_sql(query, query_purpose::create_table);
    }

    void trajectory_manager::reset_all_data() {
        trajectory_data_handling::query_handler::run_sql("DROP TABLE trajectory_information", query_purpose::reset_database);
        trajectory_data_handling::query_handler::run_sql("DROP TABLE simplified_trajectory_information", query_purpose::reset_database);
        trajectory_data_handling::query_handler::run_sql("DROP TABLE trajectory_rtree", query_purpose::reset_database);
        trajectory_data_handling::query_handler::run_sql("DROP TABLE trajectory_rtree_rowid", query_purpose::reset_database);
        trajectory_data_handling::query_handler::run_sql("DROP TABLE trajectory_rtree_parent", query_purpose::reset_database);
        trajectory_data_handling::query_handler::run_sql("DROP TABLE trajectory_rtree_node", query_purpose::reset_database);
    }
}







