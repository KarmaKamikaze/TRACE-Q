#include "trajectory.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>

using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

// Constants
const std::filesystem::path TDRIVE_PATH = std::filesystem::current_path().parent_path() / "datasets" / "t-drive";
const char DELIMITER = ',';

struct Location {
    std::string timestamp{};
    double longitude{};
    double latitude{};
};

struct Trajectory {
    unsigned id{};
    std::vector<Location> locations{};
};

//class AllTrajectories {
//    public:
//        std::vector<Trajectory> trajectories;
//};
//
//AllTrajectories allTrajectories;

std::vector<Trajectory> allTrajectories;

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

void print_trajectories() {
    for (const auto & trajectory : allTrajectories) {
        std::cout << "id: " << trajectory.id << std::endl;
        for (const auto &location: trajectory.locations) {
            std::cout << "timestamp: " << location.timestamp << std::endl;
            std::cout << "longitude, latitude: " << location.longitude << " " << location.latitude << std::endl;
        }
    }
}

void trajectorytest () {
    load_tdrive_dataset();
    print_trajectories();
}


