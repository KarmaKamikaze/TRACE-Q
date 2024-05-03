#include "Logger.hpp"

namespace logging {

    Logger::Logger(const std::string& filename) : filename(filename) {
        logfile.open(filename, std::ios::out | std::ios::app); // Append in output mode
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