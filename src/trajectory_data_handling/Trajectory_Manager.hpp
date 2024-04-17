#ifndef P8_PROJECT_TRAJECTORY_H
#define P8_PROJECT_TRAJECTORY_H

#include <vector>
#include <pqxx/pqxx>
#include "../data/Trajectory.hpp"
#include "../querying/Range_Query.hpp"
#include "../querying/KNN_Query.hpp"


namespace trajectory_data_handling {
    /**
     * An enum that can be used to specify which table in the database we wish to query.
     */
    enum class db_table {
        original_trajectories,
        simplified_trajectories
    };

    class Trajectory_Manager {
    public:

        /**
         * Inserts a given trajectory into either the original or simplified database. Also removes duplicate points before inserting.
         * @param trajectory The trajectory to insert
         * @param table The table to insert into
         */
        static void insert_trajectory(data_structures::Trajectory const& trajectory, db_table table);

        /**
         * Performs a range query on the given database given a window.
         * @param table The table to query.
         * @param window The window of the query.
         * @return A vector of trajectories that are contained i
         */
        static std::vector<data_structures::Trajectory> db_range_query(
                db_table table, spatial_queries::Range_Query::Window const& window);

        /**
         * Performs a K-Nearest-Neighbour query on the given database given a
         * @param table The table to query.
         * @param k The amount of nearest neighbours to return.
         * @param query_origin The origin point from where the nearest neighbours are discovered using euclidean distance.
         * @return A list of trajectories corresponding to the K-Nearest-Neighbours from the origin point.
         */
        static std::vector<data_structures::Trajectory> db_knn_query(
                db_table table, int k, spatial_queries::KNN_Query::KNN_Origin const& query_origin);

        /**
         * Loads a vector of trajectories from the database. If a list of ids are not given, all trajectories are loaded.
         * @param table The table to load trajectories from.
         * @param ids Vector of the ids of trajectories to load from the given table.
         * @return A vector containing the trajectories loaded from the database with the given ids.
         */
        static std::vector<data_structures::Trajectory> load_into_data_structure(
                db_table table, std::vector<unsigned int> const& ids = {});

        /**
         * Helper function that prints a list of trajectories.
         * @param all_trajectories
         */
        static void print_trajectories(std::vector<data_structures::Trajectory> const& all_trajectories);

        /**
         * Constructs the database tables and indexes.
         */
        static void create_database();

        /**
         * Drops all tables and indexes, whereafter it calls create_database to reconstruct the tables.
         */
        static void reset_all_data();
        /**
         * Converts an enum descriptor of a table into a string representation.
         * @param table The enum descriptor of the selected table.
         * @return A string corresponding to the selected table.
         */
        static std::string get_table_name(db_table table);
    private:
        /**
         * The connection string that specifies the connection details for the PostgreSQL database.
         */
        static std::string connection_string;

        /**
         * Reads a file and executes it in a given transaction on the database.
         * @param query_file_path Path to the file to execute.
         * @param transaction The transaction to execute the query on.
         */
        static void add_query_file_to_transaction(std::string const& query_file_path, pqxx::work &transaction);

        /**
         * Helper function that constructs a Location object given order, time and coordinates.
         * @param order The order of the location.
         * @param time The location's time.
         * @param coordinates A string representing the coordinates. Is parsed to two doubles.
         * @return The location object corresponding to the given parameters.
         */
        static data_structures::Location parse_location(int order, unsigned long time, std::string const& coordinates);
    };
}

#endif
