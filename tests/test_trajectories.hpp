#ifndef TRACE_Q_TEST_TRAJECTORIES_HPP
#define TRACE_Q_TEST_TRAJECTORIES_HPP

#include "../src/simp-algorithms/MRPA.hpp"

struct test_trajectories {
    bool compare_locations(data_structures::Location const& loc1, data_structures::Location const& loc2) {
        return loc1.timestamp == loc2.timestamp
               && loc1.longitude == loc2.longitude
               && loc1.latitude == loc2.latitude;
    };

    data_structures::Trajectory small{};
    data_structures::Trajectory medium{};
    data_structures::Trajectory large{};

    test_trajectories() {
        small.locations.emplace_back(data_structures::Location(1, 0, 1, 2));
        small.locations.emplace_back(data_structures::Location(2, 1, 4, 2));
        small.locations.emplace_back(data_structures::Location(3, 4, 7, 4));
        small.locations.emplace_back(data_structures::Location(4, 12, 17, 9));
        small.locations.emplace_back(data_structures::Location(5, 15, 26, 12));
        small.locations.emplace_back(data_structures::Location(6, 18, 32, 5));

        medium.locations.emplace_back(data_structures::Location(1, 0, 1, 2));
        medium.locations.emplace_back(data_structures::Location(2, 1, 4, 2));
        medium.locations.emplace_back(data_structures::Location(3, 4, 7, 4));
        medium.locations.emplace_back(data_structures::Location(4, 12, 17, 9));
        medium.locations.emplace_back(data_structures::Location(5, 15, 26, 12));
        medium.locations.emplace_back(data_structures::Location(6, 18, 32, 5));
        medium.locations.emplace_back(data_structures::Location(7, 22, 69, 3));
        medium.locations.emplace_back(data_structures::Location(8, 23, 110, 21));
        medium.locations.emplace_back(data_structures::Location(9, 24, 112, 23));

        large.locations.emplace_back(data_structures::Location(1, 0, 1, 2));
        large.locations.emplace_back(data_structures::Location(2, 1, 4, 2));
        large.locations.emplace_back(data_structures::Location(3, 4, 7, 4));
        large.locations.emplace_back(data_structures::Location(4, 12, 17, 9));
        large.locations.emplace_back(data_structures::Location(5, 15, 26, 12));
        large.locations.emplace_back(data_structures::Location(6, 18, 32, 5));
        large.locations.emplace_back(data_structures::Location(7, 22, 69, 3));
        large.locations.emplace_back(data_structures::Location(8, 23, 110, 21));
        large.locations.emplace_back(data_structures::Location(9, 24, 112, 23));
        large.locations.emplace_back(data_structures::Location(10, 28, 143, 28));
        large.locations.emplace_back(data_structures::Location(11, 30, 145, 31));
        large.locations.emplace_back(data_structures::Location(12, 35, 156, 56));
        large.locations.emplace_back(data_structures::Location(13, 37, 169, 61));
        large.locations.emplace_back(data_structures::Location(14, 41, 173, 66));
        large.locations.emplace_back(data_structures::Location(15, 44, 182, 69));
        large.locations.emplace_back(data_structures::Location(16, 50, 243, 98));
        large.locations.emplace_back(data_structures::Location(17, 53, 277, 143));
        large.locations.emplace_back(data_structures::Location(18, 60, 300, 200));
    }
};

#endif //TRACE_Q_TEST_TRAJECTORIES_HPP
