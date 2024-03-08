#include <iostream>
#include <string>
#include <format>
#include <filesystem>
#include <fstream>
#include "../data/trajectory_structure.hpp"

#include "trajectory_file_manager.h"

using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

const std::filesystem::path TDRIVE_PATH = std::filesystem::current_path().parent_path() / "external" / "datasets" /  "t-drive";
const std::filesystem::path GEOLIFE_PATH = std::filesystem::current_path().parent_path() / "external" / "datasets" / "geolife";
const char DELIMITER = ',';

void load_tdrive_dataset() {
    std::cout << TDRIVE_PATH << std::endl;
    unsigned trajectory_id = 0;
    for (const auto& dirEntry : recursive_directory_iterator(TDRIVE_PATH))  {
        std::ifstream file(dirEntry.path());

        if(file.is_open()) {
            std::string line;
            data_structures::Location location;
            data_structures::Trajectory trajectory;

            while(std::getline(file, line)) {

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
                } else {
                    std::cerr << "Error reading line: " << line << '\n';
                }
            }
            data_structures::allTrajectories.push_back(trajectory);
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
                data_structures::Location location;
                data_structures::Trajectory trajectory;
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