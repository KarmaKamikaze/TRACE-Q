#include <iostream>
#include <string>
#include <cstdio>
#include <format>
#include <cstring>
#include <cstdlib>
#include <filesystem>

#include "../../external/sqlite/sqlite3.h"

#include "../data/trajectory_structure.hpp"
#include "sqlite_querying.h"


sqlite3 *db;

using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

std::filesystem::path sqlite_db_filesystem_path = std::filesystem::current_path().parent_path() / "src" / "data" / "trajectory.db";
std::string sqlite_db_path_string = sqlite_db_filesystem_path.generic_string();
const char* sqlite_db_path = sqlite_db_path_string.c_str();

data_structures::Trajectory trajectory;
data_structures::Location location;
int currentTrajectory;
int order;

int callback(void *query_success_history, int argc, char **argv, char **azColName) {
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

    data_structures::allTrajectories.push_back(trajectory);

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

void run_sql(const char* query, query_purpose callback_type) {
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


