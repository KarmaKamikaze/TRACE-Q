#include <doctest/doctest.h>
#include "test_trajectories.hpp"
#include "../src/querying/Query.hpp"

TEST_CASE("Query - in_range") {
    auto trajectories = test_trajectories{};
    SUBCASE("in_range is true when one or more locations intersect the window") {
        auto window = spatial_queries::Query::Window{4.0, 30.0 , 2.0, 15.0, 0.0, 20.0};
        auto in_window = spatial_queries::Query::in_range(trajectories.small, window);

        CHECK(in_window);
    }

    SUBCASE("in_range is false when no locations intersect the window") {
        auto window = spatial_queries::Query::Window{50.0, 100.0, 20.0, 40.0, 25.0, 30.0};
        auto in_window = spatial_queries::Query::in_range(trajectories.small, window);

        CHECK_FALSE(in_window);
    }
}
