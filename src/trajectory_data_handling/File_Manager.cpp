#include "File_Manager.hpp"
#include "Trajectory_Manager.hpp"
#include <iostream>
#include <chrono>
#include <sstream>
#include <stdexcept>

namespace trajectory_data_handling {
    std::filesystem::path File_Manager::TDRIVE_PATH = std::filesystem::current_path().parent_path().parent_path()
            / "external" / "datasets" /  "t-drive";
    std::filesystem::path File_Manager::GEOLIFE_PATH = std::filesystem::current_path().parent_path().parent_path()
            / "external" / "datasets" / "geolife" "Geolife Trajectory 1.3" / "Data";
    char File_Manager::delimiter = ',';

    long File_Manager::stringToTime(const std::string& timeString) {
        try {
            struct tm timeStruct = {};
            std::istringstream ss(timeString);

            // Parse the year
            if (!(ss >> timeStruct.tm_year)) {
                throw std::invalid_argument("Error: Invalid year.");
            }
            timeStruct.tm_year -= 1900; // Adjust for tm_year starting from 1900

            // Parse the month, day, hour, minute, and second
            char dash;
            char colon;
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

    void File_Manager::load_tdrive_dataset() {
        for (const auto& dirEntry : recursive_directory_iterator(TDRIVE_PATH))  {
            std::ifstream file(dirEntry.path());

            if(file.is_open()) {
                std::string line{};
                data_structures::Trajectory trajectory{};
                while(std::getline(file, line)) {

                    std::istringstream lineStream(line);

                    data_structures::Location location{};
                    std::string id{};
                    std::string timestamp{};
                    std::string longitude{};
                    std::string latitude{};

                    if (std::getline(lineStream, id, delimiter) &&
                        std::getline(lineStream, timestamp, delimiter) &&
                        std::getline(lineStream, longitude, delimiter) &&
                        std::getline(lineStream, latitude, delimiter)) {
                        if (trajectory.id == 0) {
                            trajectory.id = std::stoi(id);
                        }
                        location.timestamp = File_Manager::stringToTime(timestamp);
                        location.longitude = std::stod(longitude);
                        location.latitude = std::stod(latitude);
                        trajectory.locations.push_back(location);
                    } else {
                        std::cerr << "Error reading line: " << line << '\n';
                    }
                }

                file.close();

                trajectory_data_handling::Trajectory_Manager::insert_trajectory(
                        trajectory, trajectory_data_handling::db_table::original_trajectories);

            } else {
                throw std::runtime_error("Error opening file: " + dirEntry.path().string());
            }
        }
    }

    void File_Manager::load_geolife_dataset(std::vector<data_structures::Trajectory> &all_trajectories) {
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
                    if (std::getline(lineStream, latitude, delimiter) &&
                        std::getline(lineStream, longitude, delimiter) &&
                        std::getline(lineStream, id, delimiter) &&
                        std::getline(lineStream, altitude, delimiter) &&
                        std::getline(lineStream, datedays, delimiter) &&
                        std::getline(lineStream, date, delimiter) &&
                        std::getline(lineStream, time, delimiter)) {
                        if(location.longitude == 0.0 or location.latitude == 0.0)
                            continue;
                        location.timestamp = File_Manager::stringToTime(date + " " + time);
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