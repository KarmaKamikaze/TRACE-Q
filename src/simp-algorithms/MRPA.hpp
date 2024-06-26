#ifndef TRACE_Q_MRPA_HPP
#define TRACE_Q_MRPA_HPP

#include <vector>
#include <queue>
#include "../data/Trajectory.hpp"
#include "../data/Node.hpp"

namespace simp_algorithms {

    class MRPA {
        using Trajectory = data_structures::Trajectory;
        using Node = data_structures::Node<data_structures::Location>;
        using Location = data_structures::Location;
        static constexpr auto compare =
                [](Location const &left, Location const &right) {
            return left.order < right.order;
        };
        using MRPA_PTQ = std::priority_queue<Location, std::vector<Location>, decltype(compare)>;

        /**
         * The resolution scale (called c in the MRPA paper) describes the number of the intermediate scale.
         * The lower c is, the more error tolerances we will have; thereby we have more simplified trajectories
         * to choose from. Additionally, the more error tolerances, the less the difference is between them.
         */
        double resolution_scale {2}; // c

        /**
         * Calculates a vector of error tolerances for the given trajectory based on the resolution scale and the
         * number of points in the trajectory, which describes the granularity of the resolution windows.
         * @param trajectory The trajectory which for we calculate error tolerances.
         * @return A vector of error tolerances for the given trajectory.
         */
        [[nodiscard]] std::vector<double> error_tolerance_init(Trajectory const& trajectory) const;

        /**
         * Calculates the sum of SED error over a range of an input trajectory.
         * @param trajectory The input trajectory
         * @param i Start of the range of points from the trajectory to determine the SED error sum for.
         * @param j End of the range of points from the trajectory to determine the SED error sum for.
         * @return The SED error sum for the range of points in the trajectory.
         */
        static double error_SED_sum(const Trajectory& trajectory, int i, int j);

        /**
         * Considers both spatial and temporal information on sub-trajectories to create approximate point.
         * @param trajectory Trajectory under approximation.
         * @param i Start of sub-trajectory considered for approximation.
         * @param j End of sub-trajectory considered for approximation.
         * @return The new approximation point.
         */
        static Trajectory approximated_temporally_synchronized_position(Trajectory const &trajectory, int i, int j);

        /**
         * Calculates the SED between an original point and its approximated temporal synchronized position.
         * @param original_point The original trajectory point.
         * @param approx_point The associated approximated trajectory point.
         * @return The SED error (double).
         */
        static double single_SED(Location const& original_point, Location const& approx_point);

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
        static Node init_tree(Trajectory const& trajectory, double error_tol, double high_error_tol);

        /**
         * Maintains the two priority queues working_list and future_work.
         * Also removes elements from unvisited and updates the tree structure.
         * @param tree The tree structure of the input trajectory.
         * @param trajectory Trajectory to be simplified.
         * @param error_tol The error tolerance that determines if a child is a child of a parent node.
         * @param high_error_tol A high error tolerance, which is used to skip ahead when the error becomes too high.
         * @param working_list The priority queue of vertices that are currently being processed.
         * @param future_work The priority queue of vertices that will become the working_list in the next iteration.
         * @param unvisited The set of vertices that have not yet been visited.
         * Element are removed from this set, when they are added to future_work
         */
        static void maintain_priority_queue(Node& tree, Trajectory const& trajectory, double error_tol,
                                            double high_error_tol, MRPA_PTQ& working_list, MRPA_PTQ& future_work,
                                            std::vector<Location>& unvisited);

        /**
         *
         * @param trajectory Trajectory to be simplified.
         * @param tree Tree structure from init_tree.
         * It describes the combinations of vertices that comply with the error tolerance.
         * @param error_tol The error tolerance.
         * @return The simplified trajectory
         */
        static Trajectory approximate(const Trajectory& trajectory, const Node& tree, double error_tol);

        /**
         * A helper function that ensures that init_tree is called with the correct high tolerance.
         * @param trajectory Trajectory to be simplified.
         * @param index The current index of the error_tolerances vector.
         * @param error_tolerances The vector of increasing error tolerances
         * @return The tree structure.
         */
        static Node apply_error_tolerance_scale_to_tree(Trajectory const& trajectory, int index,
                                                        const std::vector<double>& error_tolerances);

    public:

        MRPA() = default;
        explicit MRPA(double c) : resolution_scale{c} {};

        /**
         * Simplifies the input trajectory utilizing the MRPA algorithm.
         * @param trajectory The trajectory to be simplified.
         * @return A list of simplified trajectories with decreasing resolution.
         */
        std::vector<Trajectory> operator()(Trajectory const& trajectory) const;

        std::vector<std::pair<Trajectory, double>> run_get_error_tolerances(Trajectory const& trajectory) const;
    };

} // simp_algorithms

#endif //TRACE_Q_MRPA_HPP
