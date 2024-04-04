#include <algorithm>
#include "Range_Query.hpp"

namespace spatial_queries {

    bool Range_Query::in_range(data_structures::Trajectory const& trajectory, Window const& window) {
        return std::ranges::any_of(std::cbegin(trajectory.locations), std::cend(trajectory.locations),
                                   [&window](data_structures::Location const& loc){
                                       return loc.longitude >= window.x_low && loc.longitude <= window.x_high
                                              && loc.latitude >= window.y_low && loc.latitude <= window.y_high
                                              && loc.timestamp >= window.t_low && loc.timestamp <= window.t_high;
                                   });
    }

} // spatial_queries