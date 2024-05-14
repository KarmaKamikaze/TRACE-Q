#include <filesystem>
#include "Logger.hpp"

namespace logging {

    Logger::Logger(const std::string& directory, const std::string& filename) : directory(directory), filename(filename) {
        if (!std::filesystem::exists(directory)) {
            std::filesystem::create_directories(directory);
        }
        logfile.open(directory + filename, std::ios::out | std::ios::app); // Append in output mode
        if (!logfile.is_open()) {
            std::cerr << "Error opening log file: " << filename << std::endl;
        }
    }

    Logger::~Logger() {
        if (logfile.is_open()) {
            logfile.close();
        }
    }

} // logging