#include <doctest/doctest.h>
#include "test_trajectories.hpp"
#include "../src/querying/Range_Query.hpp"

TEST_CASE("Range_Query - operator()") {
    auto trajectories = test_trajectories{};

    data_structures::Trajectory simplified_small_trajectory{};
    simplified_small_trajectory.locations.emplace_back(data_structures::Location(1, 0, 1, 2));
    simplified_small_trajectory.locations.emplace_back(data_structures::Location(2, 4, 7, 4));
    simplified_small_trajectory.locations.emplace_back(data_structures::Location(3, 18, 32, 5));
    SUBCASE("operator() is true when original and simplified pass the query") {
        auto query = spatial_queries::Range_Query(trajectories.small, 4.0, 30.0 , 2.0, 15.0, 0.0, 20.0);
        auto in_window = query(simplified_small_trajectory);

        CHECK(in_window);
    }

    SUBCASE("operator() is true when original and simplified both fail the query") {
        auto query = spatial_queries::Range_Query(trajectories.small, 4.0, 30.0 , 2.0, 15.0, 20, 25);
        auto in_window = query(simplified_small_trajectory);

        CHECK(in_window);
    }

    SUBCASE("operator() is false when original passes while simplified fails the query") {
        auto query = spatial_queries::Range_Query(trajectories.small, 8, 30.0 , 2.0, 15.0, 0, 20);
        auto in_window = query(simplified_small_trajectory);

        CHECK_FALSE(in_window);
    }

}
