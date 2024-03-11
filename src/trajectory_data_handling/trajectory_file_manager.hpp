#include <iostream>
#include <string>
#include <format>
#include <filesystem>
#include <fstream>
#include "../data/trajectory_structure.hpp"

namespace Trajectory_File_Manager {
   class File_Manager {
   private:
       using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

       const std::filesystem::path TDRIVE_PATH = std::filesystem::current_path().parent_path() / "external" / "datasets" /  "t-drive";
       const std::filesystem::path GEOLIFE_PATH = std::filesystem::current_path().parent_path() / "external" / "datasets" / "geolife";
       const char DELIMITER = ',';
   public:
       void load_tdrive_dataset();
       void load_geolife_dataset();
   };
}