#include <iostream>
#include <string>
#include <format>
#include <filesystem>
#include <fstream>
#include "../data/trajectory_structure.hpp"

namespace trajectory_data_handling {
   class file_manager {
   private:
       using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

       const std::filesystem::path TDRIVE_PATH = std::filesystem::current_path().parent_path().parent_path() / "external" / "datasets" /  "t-drive";
       const std::filesystem::path GEOLIFE_PATH = std::filesystem::current_path() / "external" / "datasets" / "geolife";
       const char DELIMITER = ',';
       static long double stringToTime(const std::string& timeString);
   public:
       void load_tdrive_dataset(std::vector<data_structures::Trajectory> &all_trajectories);
       void load_geolife_dataset(std::vector<data_structures::Trajectory> &all_trajectories);
   };
}