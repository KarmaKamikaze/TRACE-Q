#include "doctest.h"

#include "../../src/trajectory_data_handling/sqlite_querying.h"

#include "../../src/trajectory_data_handling/trajectory.cpp"

namespace tests {
    // eksempel test her
    TEST_CASE("trajectory_handler tests") {
        CHECK(returntwo() == 2);
    }
}