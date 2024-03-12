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
        static constexpr auto compare = [](Trajectory::Point const &left, Trajectory::Point const &right) {
            return left.order < right.order;
        };
        using MRPA_PTQ = std::priority_queue<Trajectory::Point, std::vector<Trajectory::Point>, decltype(compare)>;
    private:
        double eps {};
        int const resolution_scale {2}; // c
        [[nodiscard]] Trajectory approximated_temporally_synchronized_position(Trajectory const &trajectory, int i, int j) const;
        /**
         * Calculates the SED between an original point and its approximated temporal synchronized position.
         * @param original_point The original trajectory point.
         * @param approx_point The associated approximated trajectory point.
         * @return The SED error (double).
         */
        [[nodiscard]] double single_SED(Trajectory::Point const& original_point, Trajectory::Point const& approx_point) const;

        /**
         * Calculates the sum of SED error over a range of an input trajectory.
         * @param trajectory The input trajectory
         * @param i Start of the range of points from the trajectory to determine the SED error sum for.
         * @param j End of the range of points from the trajectory to determine the SED error sum for.
         * @return The SED error sum for the range of points in the trajectory.
         */
        double error_SED_sum(const simp_algorithms::MRPA::Trajectory& trajectory, int i, int j);
        Node apply_error_tolerance_scale_to_tree(Trajectory const& trajectory, int index,
                                                 const std::vector<double>& error_tolerances);
        void maintain_priority_queue(Node& tree, Trajectory const& trajectory, double error_tol, double high_error_tol,
                                     MRPA_PTQ& working_list, MRPA_PTQ& future_work,
                                     std::vector<Trajectory::Point>& unvisited);
        std::vector<double> error_tolerance_init(Trajectory const& trajectory);
        Trajectory approximate(const Trajectory& trajectory, const Node& tree, double error_tol);
        /**
         * Initializes and returns a single root node, describing a tree structure.
         * A node consists of a parent and children, wherein the children are vertices of higher order, 
         * where the error SED sum of the trajectory points with range from the parent to the child comply
         * with the error tolerance.
         * @param trajectory The trajectory from which a tree structure must be made.
         * @param error_tol The error tolerance that determines if a child is a child of a parent node.
         * @param high_error_tol A high error tolerance, which is used to skip ahead when the error becomes too high.
         * @return The constructed tree.
         */
        Node init_tree(Trajectory const& trajectory, double error_tol, double high_error_tol);
    public:

        Trajectory simplify(Trajectory const&, double const&, double); // Yeet
        /**
         * Simplifies the input trajectory utilizing the MRPA algorithm.
         * @param trajectory The trajectory to be simplified.
         * @return A list of simplified trajectories with decreasing resolution.
         */
        std::vector<Trajectory> mrpa(Trajectory const& trajectory);
    };

} // simp_algorithms

#endif //TRACE_Q_MRPA_HPP
