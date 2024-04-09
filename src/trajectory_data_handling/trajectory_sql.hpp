#ifndef P8_PROJECT_TRAJECTORY_H
#define P8_PROJECT_TRAJECTORY_H

#include <vector>
#include <sqlite3.h>
#include <pqxx/pqxx>
#include "../data/trajectory_structure.hpp"
#include "sqlite_querying.hpp"
#include "../querying/Range_Query.hpp"


namespace trajectory_data_handling {
    enum class db_table {
        original_trajectories,
        simplified_trajectories
    };

    class trajectory_manager {
    public:
        void insert_trajectories(std::vector<data_structures::Trajectory> const& all_trajectories, db_table table) const;
        static void remove_from_trajectories(std::shared_ptr<std::vector<data_structures::Trajectory>>& trajectories, spatial_queries::Range_Query::Window window);
        static void spatial_range_query_on_rtree_table(query_purpose purpose, spatial_queries::Range_Query::Window window);
        static void load_into_data_structure(query_purpose purpose, std::vector<std::string> const& id);
        static void print_trajectories(std::vector<data_structures::Trajectory> &all_trajectories);
        void create_database() const;
        void reset_all_data() const;
        static void replace_trajectory(data_structures::Trajectory const& trajectory);
    private:
        static void add_query_file_to_transaction(std::string const& query_file_path, pqxx::work &transaction);
        const std::string connection_string{"user=postgres password=postgres host=localhost dbname=traceq port=5432"};
    };
}

#endif
