#include <doctest/doctest.h>
#include "test_trajectories.hpp"
#include "../src/querying/Range_Query.hpp"

TEST_CASE("Range_Query - operator()") {
    auto trajectories = test_trajectories{};
    SUBCASE("in_range is true when one or more locations intersect the window") { // DOESN'T WORK ANYMORE
        auto query = spatial_queries::Range_Query(trajectories.small, 4.0, 30.0 , 2.0, 15.0, 0.0, 20.0);
        auto in_window = query(trajectories.small);

        CHECK(in_window);
    }

    SUBCASE("in_range is false when no locations intersect the window") { // DOESN'T WORK ANYMORE
        auto query = spatial_queries::Range_Query(trajectories.small, 50.0, 100.0, 20.0, 40.0, 25.0, 30.0);
        auto in_window = query(trajectories.small);

        CHECK_FALSE(in_window);
    }
}
