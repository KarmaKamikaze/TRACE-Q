#include <iostream>
#include <cstdlib>
#include "sqlite_querying.hpp"

namespace trajectory_data_handling {
    sqlite3* query_handler::m_db{};
    data_structures::Trajectory query_handler::m_trajectory{};
    unsigned long query_handler::m_currentTrajectory = 1;
    int query_handler::m_order{};
    std::shared_ptr<std::vector<data_structures::Trajectory>> trajectory_data_handling::query_handler::original_trajectories;
    std::shared_ptr<std::vector<data_structures::Trajectory>> trajectory_data_handling::query_handler::simplified_trajectories;
    std::vector<std::string> query_handler::trajectory_ids_in_range{};



    const std::filesystem::path query_handler::m_sqlite_db_filesystem_path = std::filesystem::current_path().parent_path().parent_path() / "trajectory.db";
    const std::string query_handler::m_sqlite_db_path_string = m_sqlite_db_filesystem_path.generic_string();
    const char* query_handler::m_sqlite_db_path = m_sqlite_db_path_string.c_str();

    int query_handler::callback(void *query_success_history, int argc, char **argv, char **azColName) {
        int i;
        for(i = 0; i<argc; i++) {
            std::cout << azColName[i] << argv[i] << std::endl;
        }
        return 0;
    }

    int query_handler::callback_original_datastructure(void *query_success_history, int argc, char **argv, char **azColName) {
        char* endptr;

        auto traj_id = std::strtoul(argv[0], &endptr, 10);

        if (*endptr != '\0') {
            std::cerr << "Error: Invalid m_trajectory ID\n";
            return 1;
        }

        if (m_currentTrajectory != traj_id) {
            original_trajectories->push_back(m_trajectory);
            m_trajectory = data_structures::Trajectory{};
            m_trajectory.id = traj_id;
            m_order = 1;
            m_currentTrajectory = traj_id;
        }

        data_structures::Location location(m_order, std::strtold(argv[1], nullptr), std::stod(argv[2]), std::stod(argv[3]));

        m_trajectory.locations.push_back(location);
        m_order += 1;
        return 0;
    }

    int query_handler::callback_simplified_datastructure(void *query_success_history, int argc, char **argv, char **azColName) {
        char* endptr;

        auto traj_id = std::strtoul(argv[0], &endptr, 10);

        if (*endptr != '\0') {
            std::cerr << "Error: Invalid m_trajectory ID\n";
            return 1;
        }

        if (m_currentTrajectory != traj_id) {
            simplified_trajectories->push_back(m_trajectory);
            m_trajectory = data_structures::Trajectory{};
            m_trajectory.id = traj_id;
            m_order = 1;
            m_currentTrajectory = traj_id;
        }

        data_structures::Location location(m_order, std::strtold(argv[1], nullptr), std::stod(argv[2]), std::stod(argv[3]));

        m_trajectory.locations.push_back(location);
        m_order += 1;
        return 0;
    }

    int query_handler::callback_rtree_datastructure(void *query_success_history, int argc, char **argv,
                                                    char **azColName) {
        query_handler::trajectory_ids_in_range.emplace_back(argv[0]);
        return 0;
    }

    int query_handler::callback_original_rtree_insert(void *query_success_history, int argc, char **argv, char **azColName) {
        char *zErrMsg = nullptr;
        std::string id = argv[0];
        std::string minLongitude = argv[1];
        std::string maxLongitude = argv[2];
        std::string minLatitude = argv[3];
        std::string maxLatitude = argv[4];
        std::string minTimestamp = argv[5];
        std::string maxTimestamp = argv[6];

        std::string query = R"(INSERT INTO trajectory_rtree VALUES()"
                            + id + ", "
                            + minLongitude + ", "
                            + maxLongitude + ", "
                            + minLatitude + ", "
                            + maxLatitude + ", '"
                            + minTimestamp + "', '"
                            + maxTimestamp + "')";

        int rc = sqlite3_exec(m_db, query.c_str(), callback, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
        }
        return 0;
    }

    int query_handler::callback_simplified_rtree_insert(void *query_success_history, int argc, char **argv, char **azColName) {
        char *zErrMsg = nullptr;
        std::string id = argv[0];
        std::string minLongitude = argv[1];
        std::string maxLongitude = argv[2];
        std::string minLatitude = argv[3];
        std::string maxLatitude = argv[4];
        std::string minTimestamp = argv[5];
        std::string maxTimestamp = argv[6];

        std::string query = R"(INSERT INTO simplified_trajectory_rtree VALUES()"
                            + id + ", "
                            + minLongitude + ", "
                            + maxLongitude + ", "
                            + minLatitude + ", "
                            + maxLatitude + ", '"
                            + minTimestamp + "', '"
                            + maxTimestamp + "')";

        int rc = sqlite3_exec(m_db, query.c_str(), callback, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
        }
        return 0;
    }

    void query_handler::run_sql(const std::string& query, query_purpose callback_type) {
        char *zErrMsg = 0;
        int rc = sqlite3_open(m_sqlite_db_path, &m_db);

        if(rc) {
            std::cout << "Can't open database: " << sqlite3_errmsg(m_db) << std::endl;
            return;
        }

        switch(callback_type) {
            case query_purpose::load_original_trajectory_information_into_datastructure:
                rc = sqlite3_exec(m_db, query.c_str(), callback_original_datastructure, 0, &zErrMsg);
                original_trajectories->push_back(m_trajectory);
                break;
            case query_purpose::load_simplified_trajectory_information_into_datastructure:
                rc = sqlite3_exec(m_db, query.c_str(), callback_simplified_datastructure, 0, &zErrMsg);
                simplified_trajectories->push_back(m_trajectory);
                break;
            case query_purpose::load_rtree_into_datastructure:
                rc = sqlite3_exec(m_db, query.c_str(), callback_rtree_datastructure, 0, &zErrMsg);
                break;
            case query_purpose::insert_into_trajectory_table:
                rc = sqlite3_exec(m_db, query.c_str(), callback, 0, &zErrMsg);
                break;
            case query_purpose::insert_into_original_rtree_table:
                rc = sqlite3_exec(m_db, query.c_str(), callback_original_rtree_insert, 0, &zErrMsg);
                break;
            case query_purpose::insert_into_simplified_rtree_table:
                rc = sqlite3_exec(m_db, query.c_str(), callback_simplified_rtree_insert, 0, &zErrMsg);
                break;
            case query_purpose::create_table:
                rc = sqlite3_exec(m_db, query.c_str(), callback, 0, &zErrMsg);
                break;
            case query_purpose::reset_database:
                rc = sqlite3_exec(m_db, query.c_str(), callback, 0, &zErrMsg);
                break;
        }

        if(rc != SQLITE_OK){
            std::cout << "SQL error: " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
        }
        sqlite3_close(m_db);
    }
}

