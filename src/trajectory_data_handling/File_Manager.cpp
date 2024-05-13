#include "File_Manager.hpp"
#include "Trajectory_Manager.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <random>

namespace trajectory_data_handling {
    std::filesystem::path File_Manager::TDRIVE_PATH = std::filesystem::current_path().parent_path().parent_path()
            / "external" / "datasets" /  "t-drive";
    std::filesystem::path File_Manager::GEOLIFE_PATH = std::filesystem::current_path().parent_path().parent_path()
            / "external" / "datasets" / "geolife" / "Geolife Trajectories 1.3" / "Data";
    char File_Manager::delimiter = ',';

    unsigned long File_Manager::string_to_time(const std::string& timeString) {
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
            unsigned long secondsSinceEpoch = duration.count();

            return secondsSinceEpoch;
        } catch (const std::invalid_argument& e) {
            std::cerr << e.what() << std::endl;
            return 0;
        }
    }

    void File_Manager::load_tdrive_dataset(int number_of_files) {

        // The following values are used for data-cleaning in order to remove outliers.
        auto min_accepted_latitude = 30.69;
        auto max_accepted_latitude = 46.65;
        auto min_accepted_longitude = 104.15;
        auto max_accepted_longitude = 126.93;

        auto random_file_indexes = get_random_file_indexes(TDRIVE_PATH, number_of_files);

        int index_counter{0};
        for (const auto& dirEntry : recursive_directory_iterator(TDRIVE_PATH)) {
            if (number_of_files == 0 || random_file_indexes.contains(index_counter)) {

                std::ifstream file(dirEntry.path());

                if (file.is_open()) {
                    std::string line{};
                    data_structures::Trajectory trajectory{};
                    while (std::getline(file, line)) {

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
                            location.timestamp = File_Manager::string_to_time(timestamp);
                            location.longitude = std::stod(longitude);
                            location.latitude = std::stod(latitude);
                            if (location.latitude >= min_accepted_latitude && location.latitude <= max_accepted_latitude
                                && location.longitude >= min_accepted_longitude &&
                                location.longitude <= max_accepted_longitude) {
                                trajectory.locations.push_back(location);
                            }

                        } else {
                            std::cerr << "Error reading line: " << line << '\n';
                        }
                    }

                    file.close();

                    if (trajectory.size() != 0) {
                        trajectory_data_handling::Trajectory_Manager::insert_trajectory(
                                trajectory, trajectory_data_handling::db_table::original_trajectories);
                    }
                } else {
                    throw std::runtime_error("Error opening file: " + dirEntry.path().string());
                }
            }

            index_counter++;
        }
    }

    void File_Manager::load_geolife_dataset(int number_of_files) {

        auto random_file_indexes = get_random_file_indexes(GEOLIFE_PATH, number_of_files);

        int index_counter{0};
        unsigned trajectory_id = 1;
        for (const auto &dirEntry: recursive_directory_iterator(GEOLIFE_PATH)) {
            if (number_of_files == 0 || random_file_indexes.contains(index_counter)) {
                std::ifstream file(dirEntry.path());

                if (file.is_open()) {
                    if (!(dirEntry.path().extension() == std::string{".plt"})) {
                        continue;
                    }

                    std::string line{};
                    data_structures::Trajectory trajectory{};
                    trajectory.id = trajectory_id;
                    trajectory_id++;

                    // ignores the first 6 lines as they are irrelevant
                    for (int i = 0; i < 6; i++) {
                        std::getline(file, line);
                    }

                    while (std::getline(file, line)) {

                        std::istringstream lineStream(line);

                        data_structures::Location location{};
                        std::string latitude{};
                        std::string longitude{};
                        std::string id{};
                        std::string altitude{};
                        std::string date_days{};
                        std::string date{};
                        std::string time{};

                        if (std::getline(lineStream, latitude, delimiter) &&
                            std::getline(lineStream, longitude, delimiter) &&
                            std::getline(lineStream, id, delimiter) &&
                            std::getline(lineStream, altitude, delimiter) &&
                            std::getline(lineStream, date_days, delimiter) &&
                            std::getline(lineStream, date, delimiter) &&
                            std::getline(lineStream, time, delimiter)) {
                            location.timestamp = File_Manager::string_to_time(date + " " + time);
                            location.longitude = std::stod(longitude);
                            location.latitude = std::stod(latitude);
                            if (location.longitude == 0.0 || location.latitude == 0.0)
                                continue;
                            trajectory.locations.push_back(location);
                        } else {
                            std::cerr << "Error reading line: " << line << std::endl;
                        }
                    }
                    file.close();

                    if (!trajectory.locations.empty()) {
                        trajectory_data_handling::Trajectory_Manager::insert_trajectory(
                                trajectory, trajectory_data_handling::db_table::original_trajectories);
                    }
                } else {
                    throw std::runtime_error("Error opening file: " + dirEntry.path().string());
                }
            }

            index_counter++;
        }
    }

    std::unordered_set<int> File_Manager::get_random_file_indexes(std::filesystem::path const& path, int number_of_indexes) {
        auto file_iterator = std::filesystem::recursive_directory_iterator(path);
        auto files_in_dir = static_cast<int>(std::distance(begin(file_iterator), end(file_iterator)));
        if (number_of_indexes > files_in_dir) {
            throw std::invalid_argument("number_of_indexes cannot be more than files in directory");
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution distribution(0, files_in_dir - 1);

        std::unordered_set<int> indexes{};
        for (int i = 0; i < number_of_indexes;) {
            auto index = distribution(gen);
            if (!indexes.contains(index)) {
                indexes.insert(index);
                ++i;
            }
        }

        return indexes;
    }

}