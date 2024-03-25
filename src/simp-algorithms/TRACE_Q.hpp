#ifndef TRACE_Q_TRACE_Q_HPP
#define TRACE_Q_TRACE_Q_HPP

#include <cmath>
#include "../data/trajectory_structure.hpp"
#include "../querying/Query.hpp"

namespace trace_q {

    class TRACE_Q {
        int query_amount{};
        double grid_expansion_multiplier{};
        double grid_density_multiplier{};
        int windows_per_grid_point{};
        double window_expansion_rate{};
        double time_interval_multiplier{};
        double query_type_split{};

        [[nodiscard]] int calculate_query_amount() const;

        double query_error(data_structures::Trajectory const& trajectory,
                           std::vector<std::shared_ptr<spatial_queries::Query>> const& query_objects) const;

    public:
        TRACE_Q(double grid_expansion_multiplier, double grid_density_multiplier, int windows_per_grid_point,
                double window_expansion_rate, double time_interval_multiplier, double query_type_split)
                : grid_expansion_multiplier(grid_expansion_multiplier),
                grid_density_multiplier(grid_density_multiplier),
                windows_per_grid_point(windows_per_grid_point),
                window_expansion_rate(window_expansion_rate),
                time_interval_multiplier(time_interval_multiplier),
                query_type_split(query_type_split){
            query_amount = calculate_query_amount();
        }

        [[nodiscard]] data_structures::Trajectory simplify(const data_structures::Trajectory& original_trajectory,
                                             double max_locations, double min_query_accuracy) const;

    };

} // trace_q

#endif //TRACE_Q_TRACE_Q_HPP
