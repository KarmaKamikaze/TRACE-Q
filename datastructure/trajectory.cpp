#include "trajectory.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <fstream>
#include <cstdio>
#include <format>
#include <cstring>
#include <cstdlib>

#include "../sqlite/sqlite3.h"

using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

namespace trajectory {
    // Constants
    const std::filesystem::path TDRIVE_PATH = std::filesystem::current_path().parent_path() / "datasets" / "t-drive";
    const std::filesystem::path GEOLIFE_PATH = std::filesystem::current_path().parent_path() / "datasets" / "geolife";
    const char DELIMITER = ',';

    char const *sqlite_db_path = "../sqlite/trajectory.db";

    struct Location {
        unsigned order{};
        std::string timestamp{};
        double longitude{};
        double latitude{};
    };

    struct Trajectory {
        unsigned id{};
        std::vector<Location> locations{};
    };

    std::vector<Trajectory> allTrajectories;
    sqlite3 *db;

    static int callback(void *query_success_history, int argc, char **argv, char **azColName) {
        int i;
        for(i = 0; i<argc; i++) {
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        printf("\n");
        return 0;
    }

    int currentTrajectory = 0;
    int order{};

    static int callback_datastructure(void *query_success_history, int argc, char **argv, char **azColName) {
        Location location;
        Trajectory trajectory;

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

        allTrajectories.push_back(trajectory);

        order += 1;

        return 0;
    }

    static int callback_insert(void *query_success_history, int argc, char **argv, char **azColName) {
        char query_array[150];
        char *zErrMsg = 0;
        char* id = argv[0], *minLongitude = argv[1], *maxLongitude = argv[2], *minLatitude = argv[3], *maxLatitude = argv[4], *minTimestamp = argv[5], *maxTimestamp = argv[6];

        char *query = std::strcpy(query_array, std::format("INSERT INTO trajectory_rtree VALUES({0}, {1}, {2}, {3}, {4}, '{5}', '{6}')", id, minLongitude, maxLongitude, minLatitude, maxLatitude, minTimestamp, maxTimestamp).c_str());
        std::cout << query << '\n';
        sqlite3_exec(db, query, callback, 0, &zErrMsg);
        return 0;
    }

    void run_sql(char const *query) {

        char *zErrMsg = 0;
        int rc = sqlite3_open(sqlite_db_path, &db);

        if( rc ) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
            return;
        } else {
            fprintf(stdout, "Opened database successfully\n");
        }

        rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);

        if( rc != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            fprintf(stdout, "Table created successfully\n");
        }
        sqlite3_close(db);
    }

    void load_trajectories_into_rtree() {

        char *zErrMsg = 0;
        int rc = sqlite3_open(sqlite_db_path, &db);

        if( rc ) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
            return;
        } else {
            fprintf(stdout, "Opened database successfully\n");
        }

        char const *query = "SELECT trajectory_id, MIN(longitude) AS min_longitude, MAX(longitude) AS max_longitude,\n"
                            "MIN(latitude) AS min_latitude, MAX(latitude) AS max_latitude,\n"
                            "MIN(timestamp) AS min_timestamp, MAX(timestamp) AS max_timestamp\n"
                            "FROM trajectory_information\n"
                            "GROUP BY trajectory_id;";

        rc = sqlite3_exec(db, query, callback_insert, 0, &zErrMsg);

        if( rc != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            fprintf(stdout, "Success\n");
        }

        sqlite3_close(db);
    }

    void load_tdrive_dataset() {
        unsigned trajectory_id = 0;
        for (const auto& dirEntry : recursive_directory_iterator(TDRIVE_PATH))  {
            std::ifstream file(dirEntry.path());

            if(file.is_open()) {
                std::string line;

                while(std::getline(file, line)) {
                    Location location;
                    Trajectory trajectory;

                    std::istringstream lineStream(line);
                    std::string id, timestamp, longitude, latitude;
                    if(std::getline(lineStream, id, DELIMITER) &&
                       std::getline(lineStream, timestamp, DELIMITER) &&
                       std::getline(lineStream, longitude, DELIMITER) &&
                       std::getline(lineStream, latitude, DELIMITER)) {
                        trajectory.id = std::stoi(id);
                        location.timestamp = timestamp;
                        location.longitude = std::stod(longitude);
                        location.latitude = std::stod(latitude);
                        trajectory.locations.push_back(location);
                        allTrajectories.push_back(trajectory);

                    } else {
                        std::cerr << "Error reading line: " << line << '\n';
                    }
                }
                file.close();
                trajectory_id++;
            } else {
                throw std::runtime_error("Error opening file: " + dirEntry.path().string());
            }
        }

    }

    void load_geolife_dataset() {
        unsigned trajectory_id = 0;
        for (const auto &dirEntry: recursive_directory_iterator(GEOLIFE_PATH)) {
            std::ifstream file(dirEntry.path());

            if (file.is_open()) {
                std::string line;
                int lineCount = 0;  // Counter for lines
                while (std::getline(file, line)) {
                    ++lineCount;

                    if (lineCount <= 6) {
                        continue;  // Ignore the first 6 lines
                    }
                    Location location;
                    Trajectory trajectory;
                    std::istringstream lineStream(line);
                    std::string latitude, longitude, id, altitude, datedays, date, time;
                    if (std::getline(lineStream, latitude, DELIMITER) &&
                        std::getline(lineStream, longitude, DELIMITER) &&
                        std::getline(lineStream, id, DELIMITER) &&
                        std::getline(lineStream, altitude, DELIMITER) &&
                        std::getline(lineStream, datedays, DELIMITER) &&
                        std::getline(lineStream, date, DELIMITER) &&
                        std::getline(lineStream, time, DELIMITER)) {
                        if(location.longitude == 0.0 or location.latitude == 0.0)
                            continue;
                        location.timestamp = date + " " + time;
                        location.longitude = std::stod(longitude);
                        location.latitude = std::stod(latitude);
                        trajectory.id = std::stod(id);
                        trajectory.locations.push_back(location);
                        allTrajectories.push_back(trajectory);
                    } else {
                        std::cerr << "Error reading line: " << line << std::endl;
                    }
                }
                file.close();
                trajectory_id++;
            } else {
                throw std::runtime_error("Error opening file: " + dirEntry.path().string());
            }
        }
    }


    void insert_into_database() {
        int rc = sqlite3_open("../sqlite/trajectory.db", &db);
        char* zErrMsg = 0;
        for (const auto & trajectory : allTrajectories) {
            for (const auto &location : trajectory.locations) {
                char sql[150];
                snprintf(sql, sizeof(sql), "INSERT INTO trajectory_information VALUES(NULL, %d, '%s', %f, %f)", trajectory.id, location.timestamp.c_str(), location.longitude, location.latitude);

                // Execute the SQL query
                rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

                if( rc != SQLITE_OK ){
                    fprintf(stderr, "SQL error: %s\n", zErrMsg);
                    sqlite3_free(zErrMsg);
                }
            }
        }
        sqlite3_close(db);
    }

    void print_trajectories() {
        for (const auto & trajectory : allTrajectories) {
            std::cout << "id: " << trajectory.id << std::endl;
            for (const auto &location: trajectory.locations) {
                std::cout << "order: " << location.order << '\n';
                std::cout << "timestamp: " << location.timestamp << '\n';
                std::cout << "longitude, latitude: " << location.longitude << " " << location.latitude << '\n';
            }
        }
    }

    void load_database_into_datastructure() {
        char *zErrMsg = 0;
        int rc = sqlite3_open(sqlite_db_path, &db);

        if( rc ) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
            return;
        } else {
            fprintf(stdout, "Opened database successfully\n");
        }

        char const *query = "SELECT trajectory_id, timestamp, longitude, latitude FROM trajectory_information";

        rc = sqlite3_exec(db, query, callback_datastructure, 0, &zErrMsg);

        if( rc != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            fprintf(stdout, "Storing to datastructure: Success. Linux mode: Activated\n");
        }

        sqlite3_close(db);
    }

    int returntwo() {
        return 2;
    }

    void create_database() {
        char const *query = "CREATE TABLE trajectory_information(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, trajectory_id INTEGER NOT NULL, timestamp TEXT NOT NULL, longitude REAL NOT NULL, latitude REAL NOT NULL)";
        run_sql(query);
        query = "CREATE TABLE simplified_trajectory_information(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, trajectory_id INTEGER NOT NULL, timestamp TEXT NOT NULL, longitude REAL NOT NULL, latitude REAL NOT NULL)";
        run_sql(query);
    }

    void create_rtree_table() {
        char const *query = "CREATE VIRTUAL TABLE trajectory_rtree USING rtree(id INTEGER PRIMARY KEY, minLongitude REAL, maxLongitude REAL, minLatitude REAL, maxLatitude REAL, minTimestamp TEXT, maxTimestamp TEXT)";
        run_sql(query);
        query = "CREATE VIRTUAL TABLE simplified_trajectory_rtree USING rtree(id INTEGER PRIMARY KEY, minLongitude REAL, maxLongitude REAL, minLatitude REAL, maxLatitude REAL, minTimestamp TEXT, maxTimestamp TEXT)";
        run_sql(query);
    }

    void reset_database() {
        run_sql("DELETE FROM trajectory_information");
//        run_sql("DELETE FROM trajectory_rtree");
//        run_sql("DELETE FROM trajectory_rtree_rowid");
//        run_sql("DELETE FROM trajectory_rtree_parent");
//        run_sql("DELETE FROM trajectory_rtree_node");
    }

    void trajectorytest () {
//        reset_database();
//    create_database();
//    create_rtree_table();
//    load_tdrive_dataset();
//    insert_into_database();
//    load_geolife_dataset();
        load_database_into_datastructure();
        print_trajectories();
//        load_trajectories_into_rtree();
//        reset_database();
    }
}




