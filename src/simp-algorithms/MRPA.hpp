#ifndef TRACE_Q_MRPA_HPP
#define TRACE_Q_MRPA_HPP

#include <vector>
#include <queue>
#include "../data/Node.hpp"

namespace simp_algorithms {

    class Trajectory {
    public:
        struct Point {
            int order {-1};
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

            friend std::ostream& operator<<(std::ostream& os, const Point& p) {
                os << p.order;

                return os;
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
        Trajectory simplify_subtrajectory(Trajectory const&, int, int) const;
        double single_SED(Trajectory::Point const&, Trajectory::Point const&) const;
        double error_SED_sum(Trajectory const&, int, int);
        Node apply_error_tolerance_scale_to_tree(Trajectory const& trajectory, int index,
                                                 const std::vector<double>& error_tolerances);

        static constexpr auto compare = [](Trajectory::Point const &left, Trajectory::Point const &right) {
            return left.order > right.order;
        };

        void maintain_priority_queue(Node& tree, Trajectory const& trajectory, double error_tol, double high_error_tol,
                                     std::priority_queue<Trajectory::Point, std::vector<Trajectory::Point>, decltype(compare)>& working_list,
                                     std::priority_queue<Trajectory::Point, std::vector<Trajectory::Point>, decltype(compare)>& future_work,
                                     std::vector<Trajectory::Point>& unvisited);



    public:
        std::vector<double> error_tolerance_init(Trajectory const&); // Make private
        Trajectory simplify(Trajectory const&, double const&, double);
        std::vector<Trajectory> mrpa(Trajectory const&); // Make private
        Trajectory approximate(Trajectory const&, const Node&, double); // Make private
        Node init_tree(Trajectory const&, double, double); // Make private
        

    };

} // simp_algorithms

#endif //TRACE_Q_MRPA_HPP
