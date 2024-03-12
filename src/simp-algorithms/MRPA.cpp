#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <ranges>
#include <queue>
#include <deque>
#include <iostream>
#include <unordered_map>
#include "MRPA.hpp"

namespace simp_algorithms {
    std::vector<double> MRPA::error_tolerance_init(Trajectory const& trajectory) {
        std::vector<double> result{};

        auto number_of_tolerances = std::floor(std::log(trajectory.points.size()) / std::log(resolution_scale));

        for (auto k = 1; k <= number_of_tolerances; k++) {
            // Here we add 1 to the resolution and floor it in order to handle cases where the resolution calculation is exactly an int.
            auto resolution = std::floor(static_cast<double>(trajectory.points.size()) / std::pow(resolution_scale, k) + 1);
            double error_tolerance{};

            for (auto j = 1; j < resolution; j++) {
                auto range_start = static_cast<int>(std::floor((static_cast<double>(trajectory.points.size() - 1)) / (resolution - 1) * (j - 1) + 1));
                auto range_end = static_cast<int>(std::floor((static_cast<double>(trajectory.points.size() - 1)) / (resolution - 1) * j + 1));

                if (j == resolution - 1 && range_end < trajectory.points.size()) {
                    throw std::range_error("Last range_end variable is not equal to size of trajectory");
                }

                error_tolerance += error_SED_sum(trajectory, range_start, range_end);
            }

            error_tolerance *= 1 / (resolution - 1);
            result.emplace_back(error_tolerance);
        }

        return result;
    }

    Trajectory MRPA::approximated_temporally_synchronized_position(Trajectory const& trajectory, int i, int j) const {
        Trajectory res {};

        for (int k = i + 1; k < j; ++k) {
            auto x = trajectory.points[i].x +
                    ((trajectory.points[k].t - trajectory.points[i].t) /
                    (trajectory.points[j].t - trajectory.points[i].t)) *
                    (trajectory.points[j].x - trajectory.points[i].x);
            auto y = trajectory.points[i].y +
                    ((trajectory.points[k].t - trajectory.points[i].t) /
                     (trajectory.points[j].t - trajectory.points[i].t)) *
                     (trajectory.points[j].y - trajectory.points[i].y);
            auto point = MRPA::Trajectory::Point(trajectory.points[k].order, x, y,trajectory.points[k].t);
            res.points.emplace_back(point);
        }

        return res;
    }

    double MRPA::single_SED(Trajectory::Point const& original_point, Trajectory::Point const& approx_point) const
    {
        return sqrt(pow(original_point.x - approx_point.x, 2) + pow(original_point.y - approx_point.y, 2));
    }

    double MRPA::error_SED_sum(const simp_algorithms::MRPA::Trajectory& trajectory, int i, int j) {

        // Here we subtract one from both i and j, due to 0-indexing
        i = i - 1;
        j = j - 1;
        auto approx_trajectory = approximated_temporally_synchronized_position(trajectory, i, j);
        double res {};
        for (int k = i + 1; k < j; ++k) {
            // Here we index the approx trajectory with the difference of k and i+1.
            // This is because the approx trajectory does not have as many points,
            // therefore it does not have values in the same indexes as the original trajectory.
            res += pow(single_SED(trajectory.points[k], approx_trajectory.points[k - (i + 1)]), 2);
        }

        return res;
    }

    Trajectory MRPA::simplify(Trajectory const& trajectory, double const& query_error, double simplification_error) {

        auto approximations = mrpa(trajectory);





        return approximations.front();
    }

    std::vector<Trajectory> MRPA::mrpa(Trajectory const& trajectory) {
        std::vector<Trajectory> result{};
        auto error_tolerances = MRPA::error_tolerance_init(trajectory);

        auto first_tree = init_tree(trajectory, error_tolerances[0], error_tolerances[1]);
        auto first_approx = approximate(trajectory, first_tree, error_tolerances[0]);
        result.emplace_back(first_approx);

        for (int i = 1; i < error_tolerances.size(); ++i) {
            auto tree = apply_error_tolerance_scale_to_tree(result.back(), i, error_tolerances);
            auto approximation = approximate(result.back(), tree, error_tolerances[i]);
            result.emplace_back(approximation);
        }

        return result;
    }

    data_structures::Node<Trajectory::Point> MRPA::apply_error_tolerance_scale_to_tree(Trajectory const& trajectory,
                                                                                       int index,
                                                                                       const std::vector<double>& error_tolerances) {
        if (index == error_tolerances.size() - 1) {
            auto scale = error_tolerances[index] / error_tolerances[index - 1];
            return init_tree(trajectory, error_tolerances[index], error_tolerances[index] * scale);
        }
        else {
            return init_tree(trajectory, error_tolerances[index], error_tolerances[index + 1]);
        }
    }

    data_structures::Node<Trajectory::Point> MRPA::init_tree(Trajectory const& trajectory, double error_tol, double high_error_tol) {

        MRPA_PTQ working_list{compare};
        working_list.push(trajectory.points.front());
        MRPA_PTQ future_work(compare);
        std::vector<Trajectory::Point> unvisited{trajectory.points.begin() + 1, trajectory.points.end()};

        Node root{working_list.top()};

        while (!unvisited.empty()) {
            while (!working_list.empty()) {
                maintain_priority_queue(root, trajectory, error_tol, high_error_tol, working_list, future_work,
                                        unvisited);
            }

            while (!future_work.empty()) {
                auto fw_point = future_work.top();
                future_work.pop();
                working_list.push(fw_point);
            }
        }

        std::cout << root << "\n";

        return root;
    }

    void MRPA::maintain_priority_queue(Node& tree, Trajectory const& trajectory, double error_tol, double high_error_tol,
                                       MRPA_PTQ& working_list, MRPA_PTQ& future_work,
                                       std::vector<Trajectory::Point>& unvisited) {
        Trajectory::Point index1 = working_list.top();
        working_list.pop();

        for (int i = 0; i < unvisited.size(); ++i) {
            const auto index2 = unvisited[i];
            if (index2.order < index1.order) {
                continue;
            }
            auto error = MRPA::error_SED_sum(trajectory, index1.order, index2.order);
            std::cout << "Edge test: " << index1.order << " " << index2.order << " Error: " << error << "\n";
            if (error <= error_tol) {
                unvisited.erase(unvisited.cbegin() + i);
                i--; // decrement i because of erase
                future_work.push(index2);
                auto& parent = tree.find(index1);
                if (parent.data.order == -1) {
                    throw std::runtime_error("dummy node was returned");
                }
                auto child = Node(index2);
                parent.children.emplace_back(child);
                child.parent = std::make_shared<Node>(parent);
            }
            else if (error > high_error_tol) {
                break;
            }
        }
        // Returns through side effects in parameters
    }

    Trajectory MRPA::approximate(const Trajectory& trajectory, const Node& tree, double error_tol) {

        std::vector<double> approx_error(trajectory.points.size(), std::numeric_limits<double>::max());
        approx_error[0] = 0;

        std::unordered_map<int, Trajectory::Point> backtrack{}; // Key is point order

        auto parents = std::vector<Node>{tree};
        auto number_output_points = 1;

        while(approx_error[trajectory.points.back().order - 1] == std::numeric_limits<double>::max()) {
            std::vector<Node> children{};
            for(const auto& p : parents) {
                children.insert(children.cend(), p.children.cbegin(), p.children.cend());
            }

            for (const auto& node_index1: parents) {
                for (const auto& node_index2: children){
                    double error = error_SED_sum(trajectory, node_index1.data.order, node_index2.data.order);
                    if ((approx_error[node_index1.data.order - 1] + error < approx_error[node_index2.data.order - 1])
                    && error <= error_tol) {
                        backtrack[node_index2.data.order] = node_index1.data;
                        approx_error[node_index2.data.order - 1] = approx_error[node_index1.data.order - 1] + error;
                    }
                }
            }

            parents = std::move(children);
            number_output_points++;
        }

        std::deque<Trajectory::Point> result{trajectory.points.back()};

        for (int i = number_output_points; i >= 2; --i) {
            result.push_front(backtrack[result.front().order]);
        }

        Trajectory result_trajectory{{std::begin(result), std::end(result)}};

        // Before returning we must update the order values of the nodes.
        // This is because we use order to index, and since we return a trajectory with fewer points, we can index out of range!
        for (int i = 0; i < result_trajectory.points.size(); ++i) {
            result_trajectory.points[i].order = i + 1;
        }

        return result_trajectory;
    }

} // simp_algorithms