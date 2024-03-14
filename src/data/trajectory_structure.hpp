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
    };

//    extern std::vector<Trajectory> allTrajectories;

}

#endif //TRACE_Q_TRAJECTORY_STRUCTURE_H