//
// Created by karma on 2/22/24.
//

#ifndef TRACE_Q_MRPA_H
#define TRACE_Q_MRPA_H

#include <vector>

namespace simp_algorithms {

    class Trajectory {
    private:
        struct Point {
            double x {};
            double y {};
            double t {};
        };
    public:
        std::vector<Point> points {};
    };

    class MRPA {
        using Trajectory = simp_algorithms::Trajectory;
    private:
        double eps {};
        double resolution_scale {2}; // c
        double ISSD(Trajectory const&, Trajectory const&);
        double LSSD(Trajectory const&, Trajectory const&);
        double SED(Trajectory const&, Trajectory const&);

        std::vector<double> error_tolerance_init(Trajectory const&);
        Trajectory approximate_simplified_trajectory(Trajectory const&);
    };

} // simp_algorithms

#endif //TRACE_Q_MRPA_H
