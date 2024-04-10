#include <doctest/doctest.h>
#include <sstream>
#include "../src/data/Trajectory.hpp"

TEST_CASE("Trajectory - Size method returns correct size for location container") {

    SUBCASE("0 point trajectory") {
    auto trajectory = data_structures::Trajectory{0, std::vector<data_structures::Location>{}};

    CHECK(trajectory.size() == 0);
    }

    SUBCASE("1 point trajectory") {
        auto trajectory = data_structures::Trajectory{
            0, std::vector<data_structures::Location>{
                data_structures::Location{1, 0, 0, 0}
            }
        };

        CHECK(trajectory.size() == 1);
    }
    SUBCASE("10 point trajectory") {
        auto trajectory = data_structures::Trajectory{
                0, std::vector<data_structures::Location>{
                        data_structures::Location{1, 0, 0, 0},
                        data_structures::Location{2, 0, 0, 0},
                        data_structures::Location{3, 0, 0, 0},
                        data_structures::Location{4, 0, 0, 0},
                        data_structures::Location{5, 0, 0, 0},
                        data_structures::Location{6, 0, 0, 0},
                        data_structures::Location{7, 0, 0, 0},
                        data_structures::Location{8, 0, 0, 0},
                        data_structures::Location{9, 0, 0, 0},
                        data_structures::Location{10, 0, 0, 0}
                }
        };

        CHECK(trajectory.size() == 10);
    }
}

TEST_CASE("Trajectory - Subscripting operator returns correct element from container") {
    auto trajectory = data_structures::Trajectory{0, std::vector<data_structures::Location>{}};
    auto expected_location = data_structures::Location{1, 0, 0, 0};
    trajectory.locations.emplace_back(expected_location);

    CHECK(expected_location == trajectory[0]);
}

TEST_CASE("Location - Equal operator returns correct equality between two locations") {

    SUBCASE("Location - Two Locations with the same order, timestamp, longitude, and latitude are equal") {
        auto location1 = data_structures::Location{1, 0, 0, 0};
        auto location2 = data_structures::Location{1, 0, 0, 0};

        CHECK(location1 == location2);
    }

    SUBCASE("Location - Two Locations with different order, timestamp, longitude, and latitude are not equal") {
        auto location1 = data_structures::Location{1, 0, 0, 0};
        auto location2 = data_structures::Location{1, 2, 3, 4};

        CHECK_FALSE(location1 == location2);
    }
}

TEST_CASE("operator<< prints location order") {

    SUBCASE("One location prints one location order") {
        auto location = data_structures::Location{1, 0, 0, 0};
        auto os = std::ostringstream{};
        os << location;

        CHECK(os.str() == std::to_string(location.order));
    }

    SUBCASE("Two locations prints two location orders") {
        auto location1 = data_structures::Location{1, 0, 0, 0};
        auto location2 = data_structures::Location{2, 0, 0, 0};
        auto os = std::ostringstream{};
        os << location1 << location2;

        CHECK(os.str() == std::to_string(location1.order) + std::to_string(location2.order));
    }
}
