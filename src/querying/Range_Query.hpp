#ifndef TRACE_Q_RANGE_QUERY_HPP
#define TRACE_Q_RANGE_QUERY_HPP

#include "Query.hpp"

namespace spatial_queries {

    class Range_Query : public Query {
        struct Window {
            double x_low{};
            double x_high{};
            double y_low{};
            double y_high{};
            double t_low{};
            double t_high{};
        };

        Window window{};
        bool original_in_window{};

        [[nodiscard]] bool in_range(data_structures::Trajectory const& trajectory) const;

    public:

        Range_Query(const data_structures::Trajectory& original_trajectory, double x_low, double x_high, double y_low,
                    double y_high, double t_low, double t_high) : window{x_low, x_high, y_low, y_high, t_low, t_high}
        {
            original_in_window = in_range(original_trajectory);
        }

        bool operator()(data_structures::Trajectory const& trajectory) override;

        ~Range_Query() override = default;

    };

} // spatial_queries

#endif //TRACE_Q_RANGE_QUERY_HPP
