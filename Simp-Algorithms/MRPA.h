//
// Created by karma on 2/22/24.
//

#ifndef TRACE_Q_MRPA_H
#define TRACE_Q_MRPA_H

#include <vector>
#include <queue>
#include "../datastructure/Node.h"

namespace simp_algorithms {

    class Trajectory {
    public:
        struct Point {
            int order {};
            double x {};
            double y {};
            double t {};
            bool operator==(const Point& other) const {
                if (this->order == other.order && this->x == other.x && this->y == other.y && this->t == other.t) {
                    return true;
                }
                else {
                    return false;
                }
            }
        };
        std::vector<Point> points {};
    };

    class MRPA {
        using Trajectory = simp_algorithms::Trajectory;
        using Node = data_structures::Node<Trajectory::Point>;
    private:
        double eps {};
        int const resolution_scale {3}; // c
        Trajectory simplify_subtrajectory(Trajectory const&, int, int);
        double single_SED(Trajectory::Point const&, Trajectory::Point const&);
        double error_SED_sum(Trajectory const&, int, int);
        Node init_tree(Trajectory const&, double, double);
        Node apply_error_tolerance_scale_to_tree(Trajectory const& trajectory, int index,
                                                 const std::vector<double>& error_tolerances);
        void maintain_priority_queue(Node& tree, Trajectory const& trajectory, double error_tol, double high_error_tol,
                                     std::deque<Trajectory::Point>& working_list,
                                     std::deque<Trajectory::Point>& future_work,
                                     std::vector<Trajectory::Point>& unvisited);


    public:
        std::vector<double> error_tolerance_init(Trajectory const&); // Make private
        Trajectory simplify(Trajectory const&, double const&, double); // Make private
        std::vector<Trajectory> mrpa(Trajectory const&); // Make private
        Trajectory approximate(Trajectory const&, const Node&, double);
        

    };

} // simp_algorithms

#endif //TRACE_Q_MRPA_H
