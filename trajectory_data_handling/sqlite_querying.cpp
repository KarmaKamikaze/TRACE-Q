#include <iostream>
#include <string>
#include <cstdio>
#include <format>
#include <cstring>
#include <cstdlib>

#include "../sqlite/sqlite3.h"
#include "trajectory.h"

namespace sqlite_querying {
    sqlite3 *db;

    class DatabaseManager {
    private:
        char const *sqlite_db_path = "../sqlite/trajectory.db";
        static TrajectoryManager trajectoryManager;

        static Trajectory trajectory;
        static Location location;
        static int currentTrajectory;
        static int order;
    public:
        enum query_purpose { insert_into_trajectory_table, insert_into_rtree_table, load_trajectory_information_into_datastructure, create_table, reset_database};

        static int callback(void *query_success_history, int argc, char **argv, char **azColName) {
            int i;
            for(i = 0; i<argc; i++) {
                printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
            }
            printf("\n");
            return 0;
        }

        static int callback_datastructure(void *query_success_history, int argc, char **argv, char **azColName) {
            char* endptr;
            trajectory.id = std::strtoul(argv[0], &endptr, 10);

            if (*endptr != '\0') {
                std::cerr << "Error: Invalid trajectory ID\n";
                return 1;
            }


            if (currentTrajectory != trajectory.id) {
                order = 1;
            }

            currentTrajectory = trajectory.id;
            location.order = order;
            location.timestamp = argv[1];
            location.longitude = std::stod(argv[2]);
            location.latitude = std::stod(argv[3]);
            trajectory.locations.push_back(location);

            trajectoryManager.allTrajectories.push_back(trajectory);

            order += 1;

            return 0;
        }

        static int callback_rtree_insert(void *query_success_history, int argc, char **argv, char **azColName) {
            char query_array[150];
            char *zErrMsg = 0;
            char* id = argv[0], *minLongitude = argv[1], *maxLongitude = argv[2], *minLatitude = argv[3], *maxLatitude = argv[4], *minTimestamp = argv[5], *maxTimestamp = argv[6];

            char *query = std::strcpy(query_array, std::format("INSERT INTO trajectory_rtree VALUES({0}, {1}, {2}, {3}, {4}, '{5}', '{6}')", id, minLongitude, maxLongitude, minLatitude, maxLatitude, minTimestamp, maxTimestamp).c_str());
            std::cout << query << '\n';
            sqlite3_exec(db, query, callback, 0, &zErrMsg);
            return 0;
        }

        void run_sql(char const *query, int callback_type) {
            char *zErrMsg = 0;
            int rc = sqlite3_open(sqlite_db_path, &db);

            if(rc) {
                fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
                return;
            }

            switch(callback_type) {
                case load_trajectory_information_into_datastructure:
                    rc = sqlite3_exec(db, query, callback_datastructure, 0, &zErrMsg);
                    break;
                case insert_into_trajectory_table:
                    rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
                    break;
                case insert_into_rtree_table:
                    rc = sqlite3_exec(db, query, callback_rtree_insert, 0, &zErrMsg);
                    break;
                case create_table:
                    rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
                    break;
                case reset_database:
                    rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
                    break;
            }

            if(rc != SQLITE_OK){
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
            }

            sqlite3_close(db);
        }


        void create_database() {
            char const *query = "CREATE TABLE trajectory_information(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, trajectory_id INTEGER NOT NULL, timestamp TEXT NOT NULL, longitude REAL NOT NULL, latitude REAL NOT NULL)";
            run_sql(query, create_table);
            query = "CREATE TABLE simplified_trajectory_information(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, trajectory_id INTEGER NOT NULL, timestamp TEXT NOT NULL, longitude REAL NOT NULL, latitude REAL NOT NULL)";
            run_sql(query, create_table);
        }

        void create_rtree_table() {
            char const *query = "CREATE VIRTUAL TABLE trajectory_rtree USING rtree(id INTEGER PRIMARY KEY, minLongitude REAL, maxLongitude REAL, minLatitude REAL, maxLatitude REAL, minTimestamp TEXT, maxTimestamp TEXT)";
            run_sql(query, create_table);
            query = "CREATE VIRTUAL TABLE simplified_trajectory_rtree USING rtree(id INTEGER PRIMARY KEY, minLongitude REAL, maxLongitude REAL, minLatitude REAL, maxLatitude REAL, minTimestamp TEXT, maxTimestamp TEXT)";
            run_sql(query, create_table);
        }

        void reset_all_data() {
            run_sql("DELETE FROM trajectory_information", reset_database);
            run_sql("DELETE FROM trajectory_rtree", reset_database);
            run_sql("DELETE FROM trajectory_rtree_rowid", reset_database);
            run_sql("DELETE FROM trajectory_rtree_parent", reset_database);
            run_sql("DELETE FROM trajectory_rtree_node", reset_database);
        }
    };

}