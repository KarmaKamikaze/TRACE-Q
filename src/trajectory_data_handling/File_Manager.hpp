#include <iostream>
#include <string>
#include <format>
#include <filesystem>
#include <fstream>
#include "../data/Trajectory.hpp"

namespace trajectory_data_handling {
   class File_Manager {
   private:
       using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

       static std::filesystem::path TDRIVE_PATH;
       static std::filesystem::path GEOLIFE_PATH;
       static char delimiter;
   public:
       static long stringToTime(const std::string& timeString);
       static void load_tdrive_dataset();
       static void load_geolife_dataset();
       static unsigned long string_to_time(const std::string& timeString);
   };
}