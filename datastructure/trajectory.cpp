#include "trajectory.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <fstream>
#include <cstdio>

#include "../sqlite/sqlite3.h"

using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

namespace trajectory {
    // Constants
    const std::filesystem::path TDRIVE_PATH = std::filesystem::current_path().parent_path() / "datasets" / "t-drive";
    const std::filesystem::path GEOLIFE_PATH = std::filesystem::current_path().parent_path() / "datasets" / "geolife";
    const char DELIMITER = ',';

    char const *sqlite_db_path = "../sqlite/trajectory.db";

    struct Location {
        std::string timestamp{};
        double longitude{};
        double latitude{};
    };

    struct Trajectory {
        unsigned id{};
        std::vector<Location> locations{};
    };

    std::vector<Trajectory> allTrajectories;

    static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
        int i;
        for(i = 0; i<argc; i++) {
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        printf("\n");
        return 0;
    }


    void run_sql(char const *query) {
        sqlite3 *db;
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
        sqlite3 *db;
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

        rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
        std::cout << rc;

        if( rc != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            fprintf(stdout, "Table created successfully\n");
        }

        // TODO: loop over rc og indsæt det i rtree table

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
                        trajectory.id = trajectory_id;
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
                        location.timestamp = date + " " + time;
                        location.longitude = std::stod(longitude);
                        location.latitude = std::stod(latitude);
                        trajectory.id = trajectory_id;
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
        sqlite3 *db;
        int rc = sqlite3_open("../sqlite/trajectory.db", &db);
        char* zErrMsg = 0;
        for (const auto & trajectory : allTrajectories) {
            for (const auto &location : trajectory.locations) {
                // Allocate memory for the SQL query
                char sql[512]; // Adjust the size as needed
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
                std::cout << "timestamp: " << location.timestamp << std::endl;
                std::cout << "longitude, latitude: " << location.longitude << " " << location.latitude << std::endl;
            }
        }
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

    void trajectorytest () {
//    create_database();
//    create_rtree_table();
//    load_tdrive_dataset();
//    insert_into_database();
//    load_geolife_dataset();
//    print_trajectories();
        load_trajectories_into_rtree();
    }
}




