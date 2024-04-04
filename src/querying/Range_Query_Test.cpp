#include <algorithm>
#include "Range_Query_Test.hpp"

namespace spatial_queries {
        bool Range_Query_Test::operator()(data_structures::Trajectory const& trajectory) {
        return Range_Query::in_range(trajectory, window) == original_in_window;
    }

} // spatial_queries