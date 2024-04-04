#include <algorithm>
#include "Range_Query_Test.hpp"

namespace spatial_queries {
    bool Range_Query_Test::in_range(data_structures::Trajectory const& trajectory) const {
        return std::ranges::any_of(std::cbegin(trajectory.locations), std::cend(trajectory.locations),
                                   [this](data_structures::Location const& loc){
                                       return loc.longitude >= window.x_low && loc.longitude <= window.x_high
                                              && loc.latitude >= window.y_low && loc.latitude <= window.y_high
                                              && loc.timestamp >= window.t_low && loc.timestamp <= window.t_high;
                                   });

    }

    bool Range_Query_Test::operator()(data_structures::Trajectory const& trajectory) {
        return in_range(trajectory) == original_in_window;
    }

} // spatial_queries