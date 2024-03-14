#include "sqlite_querying.hpp"
#include <iostream>
#include <cstdlib>

namespace sqlite_querying {
    sqlite3* query_handler::m_db{};
    data_structures::Trajectory query_handler::m_trajectory{};
    unsigned long query_handler::m_currentTrajectory = 1;
    int query_handler::m_order{};
    std::shared_ptr<std::vector<data_structures::Trajectory>> sqlite_querying::query_handler::all_trajectories;


    const std::filesystem::path query_handler::m_sqlite_db_filesystem_path = std::filesystem::current_path().parent_path() / "src" / "data" / "trajectory.db";
    const std::string query_handler::m_sqlite_db_path_string = m_sqlite_db_filesystem_path.generic_string();
    const char* query_handler::m_sqlite_db_path = m_sqlite_db_path_string.c_str();

    int query_handler::callback(void *query_success_history, int argc, char **argv, char **azColName) {
        int i;
        for(i = 0; i<argc; i++) {
            std::cout << azColName[i] << argv[i] << std::endl;
        }
        return 0;
    }

    int query_handler::callback_datastructure(void *query_success_history, int argc, char **argv, char **azColName) {
        char* endptr;

        auto traj_id = std::strtoul(argv[0], &endptr, 10);

        if (*endptr != '\0') {
            std::cerr << "Error: Invalid m_trajectory ID\n";
            return 1;
        }

        if (m_currentTrajectory != traj_id) {
            all_trajectories->push_back(m_trajectory);
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

    int query_handler::callback_rtree_insert(void *query_success_history, int argc, char **argv, char **azColName) {
        char *zErrMsg = nullptr;
        const char* id = argv[0];
        const char* minLongitude = argv[1];
        const char* maxLongitude = argv[2];
        const char* minLatitude = argv[3];
        const char* maxLatitude = argv[4];
        const char* minTimestamp = argv[5];
        const char* maxTimestamp = argv[6];

        // Construct the SQL query using raw string literals and std::to_string()
        std::string query = R"(INSERT INTO trajectory_rtree VALUES()"
                            + std::string(id) + ", "
                            + std::string(minLongitude) + ", "
                            + std::string(maxLongitude) + ", "
                            + std::string(minLatitude) + ", "
                            + std::string(maxLatitude) + ", '"
                            + std::string(minTimestamp) + "', '"
                            + std::string(maxTimestamp) + "')";

        std::cout << query << '\n';

        // Execute the SQL query
        int rc = sqlite3_exec(m_db, query.c_str(), callback, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
        }
        return 0;
    }

    void query_handler::run_sql(const char* query, query_purpose callback_type) {
        char *zErrMsg = 0;
        int rc = sqlite3_open(m_sqlite_db_path, &m_db);

        if(rc) {
            std::cout << "Can't open database: " << sqlite3_errmsg(m_db) << std::endl;
            return;
        }

        switch(callback_type) {
            case load_trajectory_information_into_datastructure:
                rc = sqlite3_exec(m_db, query, callback_datastructure, 0, &zErrMsg);
                all_trajectories->push_back(m_trajectory);
                break;
            case insert_into_trajectory_table:
                rc = sqlite3_exec(m_db, query, callback, 0, &zErrMsg);
                break;
            case insert_into_rtree_table:
                rc = sqlite3_exec(m_db, query, callback_rtree_insert, 0, &zErrMsg);
                break;
            case create_table:
                rc = sqlite3_exec(m_db, query, callback, 0, &zErrMsg);
                break;
            case reset_database:
                rc = sqlite3_exec(m_db, query, callback, 0, &zErrMsg);
                break;
        }

        if(rc != SQLITE_OK){
            std::cout << "SQL error: " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
        }
        sqlite3_close(m_db);
    }
}

