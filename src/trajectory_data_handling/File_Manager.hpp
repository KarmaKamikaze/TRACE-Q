#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <unordered_set>
#include "../data/Trajectory.hpp"

namespace trajectory_data_handling {

   class File_Manager {
   private:
       using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

       static std::filesystem::path TDRIVE_PATH;
       static std::filesystem::path GEOLIFE_PATH;
       static char delimiter;

       static std::unordered_set<int> get_random_file_indexes(std::filesystem::path const& path, int number_of_indexes);
   public:
       /**
        * Loads the T-Drive dataset files into the database.
        * @param number_of_files The number of random files to fetch. If number_of_indexes = 0, all files are loaded.
        */
       static void load_tdrive_dataset(int number_of_files = 0);

       /**
        * Loads the Geolife dataset files into the database.
        * @param number_of_files The number of random files to fetch. If number_of_indexes = 0, all files are loaded.
        */
       static void load_geolife_dataset(int number_of_files = 0);

       static unsigned long string_to_time(const std::string& timeString);
   };
}