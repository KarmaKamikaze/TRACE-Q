#include "doctest.h"

#include "../../src/trajectory_data_handling/trajectory_sql.cpp"
#include "../../src/trajectory_data_handling/trajectory_file_manager.hpp"


namespace tests {
    // eksempel test her
    TEST_CASE("trajectory_handler tests") {
        CHECK(returntwo() == 2);
    }
}