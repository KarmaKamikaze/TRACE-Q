#ifndef TRACE_Q_QUERY_HANDLER_HPP
#define TRACE_Q_QUERY_HANDLER_HPP

#include <iostream>
#include <string>
#include <cstdio>
#include <format>
#include <cstring>
#include <cstdlib>
#include <filesystem>
#include <sqlite3.h>

#include "../data/Trajectory.hpp"

namespace trajectory_data_handling {
    enum class query_purpose {
        insert_into_trajectory_table,
        insert_into_original_rtree_table,
        insert_into_simplified_rtree_table,
        load_original_trajectory_information_into_data_structure,
        load_simplified_trajectory_information_into_data_structure,
        load_original_rtree_into_datastructure,
        load_simplified_rtree_into_datastructure,
        create_table,
        reset_database};

    class Query_Handler {
    private:
        static sqlite3 *m_db;
        using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

        static const std::filesystem::path m_sqlite_db_filesystem_path;
        static const std::string m_sqlite_db_path_string;
        static const char* m_sqlite_db_path;

        static data_structures::Trajectory m_trajectory;
        static unsigned long m_currentTrajectory;
        static int m_order;
        static int callback(void *query_success_history, int argc, char **argv, char **azColName);
        static int callback_original_datastructure(void *query_success_history, int argc, char **argv, char **azColName);
        static int callback_simplified_datastructure(void *query_success_history, int argc, char **argv, char **azColName);
        static int callback_rtree_datastructure(void *query_success_history, int argc, char **argv, char **azColName);
        static int callback_original_rtree_insert(void *query_success_history, int argc, char **argv, char **azColName);
        static int callback_simplified_rtree_insert(void *query_success_history, int argc, char **argv, char **azColName);
    public:
        static std::shared_ptr<std::vector<data_structures::Trajectory>> original_trajectories;
        static std::shared_ptr<std::vector<data_structures::Trajectory>> simplified_trajectories;
        static std::vector<std::string> trajectory_ids_in_range;
        static void run_sql(const std::string& query, query_purpose callback_type);
    };
}
#endif
