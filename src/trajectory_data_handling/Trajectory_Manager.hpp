#ifndef P8_PROJECT_TRAJECTORY_H
#define P8_PROJECT_TRAJECTORY_H

#include <vector>
#include <pqxx/pqxx>
#include "../data/Trajectory.hpp"
#include "../querying/Range_Query.hpp"


namespace trajectory_data_handling {
    enum class db_table {
        original_trajectories,
        simplified_trajectories
    };

    class Trajectory_Manager {
    public:
        static void insert_trajectory(data_structures::Trajectory const& trajectory, db_table table);
        static std::vector<data_structures::Trajectory> db_range_query(db_table table, spatial_queries::Range_Query::Window const& window);
        static std::vector<data_structures::Trajectory> load_into_data_structure(db_table table, std::vector<int> const& ids = {});
        static void print_trajectories(std::vector<data_structures::Trajectory> const& all_trajectories);
        static void create_database();
        static void reset_all_data();
        static std::vector<data_structures::Trajectory> db_knn_query(
                db_table table, int k, std::tuple<double, double, unsigned long, unsigned long> query_origin);
        static void replace_trajectory(data_structures::Trajectory const& trajectory);
    private:
        static std::string connection_string;
        static void add_query_file_to_transaction(std::string const& query_file_path, pqxx::work &transaction);
        static data_structures::Location parse_location(int order, unsigned long time, std::string const& coordinates);
        static std::string get_table_name(db_table table);
    };
}

#endif
