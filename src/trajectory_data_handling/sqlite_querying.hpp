#ifndef TRACE_Q_SQLITE_QUERYING_HPP
#define TRACE_Q_SQLITE_QUERYING_HPP

#include <iostream>
#include <string>
#include <cstdio>
#include <format>
#include <cstring>
#include <cstdlib>
#include <filesystem>

#include "../data/trajectory_structure.hpp"
#include "../../external/sqlite/sqlite3.h"

namespace sqlite_querying {
    enum query_purpose { insert_into_trajectory_table, insert_into_rtree_table, load_trajectory_information_into_datastructure, create_table, reset_database};

    class query_handler {
    private:
        static sqlite3 *m_db;
        using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

        static const std::filesystem::path m_sqlite_db_filesystem_path;
        static const std::string m_sqlite_db_path_string;
        static const char* m_sqlite_db_path;

        static data_structures::Trajectory m_trajectory;
        data_structures::Location m_location;
        static int m_currentTrajectory;
        static int m_order;
        static int callback(void *query_success_history, int argc, char **argv, char **azColName);
        static int callback_datastructure(void *query_success_history, int argc, char **argv, char **azColName);
        static int callback_rtree_insert(void *query_success_history, int argc, char **argv, char **azColName);
    public:
        static std::shared_ptr<std::vector<data_structures::Trajectory>>  all_trajectories;
        static void run_sql(const char* query, query_purpose callback_type);
    };
}
#endif
