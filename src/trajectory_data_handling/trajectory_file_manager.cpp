#include "trajectory_file_manager.hpp"
#include <iostream>
#include <chrono>
#include <sstream>
#include <stdexcept>

namespace trajectory_data_handling {
    long file_manager::stringToTime(const std::string& timeString) {
        try {
            struct tm timeStruct = {};
            std::istringstream ss(timeString);

            // Parse the year
            if (!(ss >> timeStruct.tm_year)) {
                throw std::invalid_argument("Error: Invalid year.");
            }
            timeStruct.tm_year -= 1900; // Adjust for tm_year starting from 1900

            // Parse the month, day, hour, minute, and second
            char dash, colon;
            if (!(ss >> dash >> timeStruct.tm_mon >> dash >> timeStruct.tm_mday >> timeStruct.tm_hour >> colon
            >> timeStruct.tm_min >> colon >> timeStruct.tm_sec)) {
                throw std::invalid_argument("Error: Invalid date and time format.");
            }
            timeStruct.tm_mon--; // Adjust for tm_mon starting from 0

            // Convert std::tm to time_t
            std::time_t timeT = std::mktime(&timeStruct);

            // Convert time_t to std::chrono::system_clock::time_point
            auto timePoint = std::chrono::system_clock::from_time_t(timeT);

            // Convert to seconds since epoch
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(timePoint.time_since_epoch());
            long secondsSinceEpoch = duration.count();

            return secondsSinceEpoch;
        } catch (const std::invalid_argument& e) {
            std::cerr << e.what() << std::endl;
            return 0.0;
        }
    }

    void file_manager::load_tdrive_dataset(std::vector<data_structures::Trajectory> &all_trajectories) {
        std::cout << TDRIVE_PATH << std::endl;
        unsigned trajectory_id = 0;
        for (const auto& dirEntry : recursive_directory_iterator(TDRIVE_PATH))  {
            std::ifstream file(dirEntry.path());

            if(file.is_open()) {
                std::string line{};
                data_structures::Location location{};
                data_structures::Trajectory trajectory{};

                while(std::getline(file, line)) {

                    std::istringstream lineStream(line);
                    std::string id, timestamp, longitude, latitude;
                    if(std::getline(lineStream, id, DELIMITER) &&
                       std::getline(lineStream, timestamp, DELIMITER) &&
                       std::getline(lineStream, longitude, DELIMITER) &&
                       std::getline(lineStream, latitude, DELIMITER)) {
                        trajectory.id = std::stoi(id);
                        location.timestamp = file_manager::stringToTime(timestamp);
                        location.longitude = std::stod(longitude);
                        location.latitude = std::stod(latitude);
                        trajectory.locations.push_back(location);
                    } else {
                        std::cerr << "Error reading line: " << line << '\n';
                    }
                }
                all_trajectories.push_back(trajectory);
                file.close();
                trajectory_id++;
            } else {
                throw std::runtime_error("Error opening file: " + dirEntry.path().string());
            }
        }
    }

    void file_manager::load_geolife_dataset(std::vector<data_structures::Trajectory> &all_trajectories) {
        unsigned trajectory_id = 0;

        for (const auto &dirEntry: recursive_directory_iterator(GEOLIFE_PATH)) {
            std::ifstream file(dirEntry.path());

            if (file.is_open()) {
                std::string line{};
                data_structures::Location location{};
                data_structures::Trajectory trajectory{};

                // ignores the first 6 lines as they are irrelevant
                for(int i = 0; i < 6; i++) {
                    std::getline(file, line);
                }

                while (std::getline(file, line)) {
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
                        location.timestamp = file_manager::stringToTime(date + " " + time);
                        location.longitude = std::stod(longitude);
                        location.latitude = std::stod(latitude);
                        trajectory.id = std::stod(id);
                        trajectory.locations.push_back(location);
                    } else {
                        std::cerr << "Error reading line: " << line << std::endl;
                    }
                }
                all_trajectories.push_back(trajectory);
                file.close();
                trajectory_id++;
            } else {
                throw std::runtime_error("Error opening file: " + dirEntry.path().string());
            }
        }
    }
}