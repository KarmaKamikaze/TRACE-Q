#ifndef TRACE_Q_LOGGER_HPP
#define TRACE_Q_LOGGER_HPP

#include <string>
#include <fstream>
#include <iostream>

namespace logging {

    class Logger {
    private:
        std::string directory;
        std::string filename;
        std::ofstream logfile;

    public:
        explicit Logger(const std::string& directory, const std::string& filename);

        ~Logger();

        template <typename T>
        Logger& operator<<(const T& value) {
            if (logfile.is_open()) {
                logfile << value << "\n";
                std::cout << value << std::endl;
            }
            return *this;
        }

    };

} // logging

#endif //TRACE_Q_LOGGER_HPP
