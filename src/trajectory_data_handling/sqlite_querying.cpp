#include "sqlite_querying.hpp"

namespace SQLite_Querying {
    sqlite3* Query_Handler::m_db{};
    data_structures::Trajectory Query_Handler::m_trajectory;
    data_structures::Location Query_Handler::m_location;
    int Query_Handler::m_currentTrajectory{};
    int Query_Handler::m_order{};

    int Query_Handler::callback(void *query_success_history, int argc, char **argv, char **azColName) {
        int i;
        for(i = 0; i<argc; i++) {
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        printf("\n");
        return 0;
    }

    int Query_Handler::callback_datastructure(void *query_success_history, int argc, char **argv, char **azColName) {
        char* endptr;
        m_trajectory.id = std::strtoul(argv[0], &endptr, 10);

        if (*endptr != '\0') {
            std::cerr << "Error: Invalid m_trajectory ID\n";
            return 1;
        }


        if (m_currentTrajectory != m_trajectory.id) {
            m_order = 1;
        }

        m_currentTrajectory = m_trajectory.id;
        m_location.order = m_order;
        m_location.timestamp = argv[1];
        m_location.longitude = std::stod(argv[2]);
        m_location.latitude = std::stod(argv[3]);
        m_trajectory.locations.push_back(m_location);

        data_structures::allTrajectories.push_back(m_trajectory);

        m_order += 1;

        return 0;
    }

    int Query_Handler::callback_rtree_insert(void *query_success_history, int argc, char **argv, char **azColName) {
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

    void Query_Handler::run_sql(const char* query, query_purpose callback_type) {
        char *zErrMsg = 0;
        int rc = sqlite3_open(m_sqlite_db_path, &m_db);

        if(rc) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(m_db));
            return;
        }

        switch(callback_type) {
            case load_trajectory_information_into_datastructure:
                rc = sqlite3_exec(m_db, query, callback_datastructure, 0, &zErrMsg);
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
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }

        sqlite3_close(m_db);
    }
}

