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
#include "../sqlite/sqlite_querying.h"

using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

namespace trajectory {
//    void load_database_into_datastructure() {
//        char *zErrMsg = 0;
//        int rc = sqlite3_open(sqlite_db_path, &db);
//
//        if( rc ) {
//            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
//            return;
//        } else {
//            fprintf(stdout, "Opened database successfully\n");
//        }
//
//        char const *query = "SELECT trajectory_id, timestamp, longitude, latitude FROM trajectory_information";
//
//        rc = sqlite3_exec(db, query, callback_datastructure, 0, &zErrMsg);
//
//        if( rc != SQLITE_OK ){
//            fprintf(stderr, "SQL error: %s\n", zErrMsg);
//            sqlite3_free(zErrMsg);
//        } else {
//            fprintf(stdout, "Storing to datastructure: Success. Linux mode: Activated\n");
//        }
//
//        sqlite3_close(db);
//    }
};


    const std::filesystem::path TDRIVE_PATH = std::filesystem::current_path().parent_path() / "datasets" / "t-drive";
    const std::filesystem::path GEOLIFE_PATH = std::filesystem::current_path().parent_path() / "datasets" / "geolife";
    const char DELIMITER = ',';

    char const *sqlite_db_path = "../sqlite/trajectory.db";

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




