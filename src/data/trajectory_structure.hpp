#ifndef TRACE_Q_TRAJECTORY_STRUCTURE_H
#define TRACE_Q_TRAJECTORY_STRUCTURE_H

#include <vector>
#include <string>
namespace data_structures {

    struct Location {
        unsigned order{};
        long double timestamp{};
        double longitude{};
        double latitude{};
    };

    struct Trajectory {
        unsigned id{};
        std::vector<Location> locations{};

        Location& operator[] (int index) {
            return locations[index];
        }
    };

}

#endif //TRACE_Q_TRAJECTORY_STRUCTURE_H