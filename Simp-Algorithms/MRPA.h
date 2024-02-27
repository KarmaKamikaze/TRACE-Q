//
// Created by karma on 2/22/24.
//

#ifndef TRACE_Q_MRPA_H
#define TRACE_Q_MRPA_H

#include <vector>

namespace simp_algorithms {

    class Trajectory {
    public:
        struct Point {
            double x {};
            double y {};
            double t {};
        };
        std::vector<Point> points {};
    };

    class MRPA {
        using Trajectory = simp_algorithms::Trajectory;
    private:
        double eps {};
        double resolution_scale {2}; // c
        double ISSD(Trajectory const&, Trajectory const&);
        double LSSD(Trajectory const&, Trajectory const&);
        double single_SED(Trajectory::Point const&, Trajectory::Point const&);


        std::vector<double> error_tolerance_init(Trajectory const&);

    public:
        Trajectory simplify_subtrajectory(Trajectory const&, int, int); // Make private
        double error_SED_sum(Trajectory const&, int, int); // Make private

    };

} // simp_algorithms

#endif //TRACE_Q_MRPA_H
