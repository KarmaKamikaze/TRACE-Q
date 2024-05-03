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

    std::vector<std::pair<data_structures::Trajectory, double>> MRPA::run_get_error_tolerances(data_structures::Trajectory const& trajectory) const {
        if(resolution_scale > static_cast<double>(trajectory.size())) {
            throw std::invalid_argument("resolution_scale is larger than the trajectory's size");
        }

        std::vector<std::pair<Trajectory, double>> result{};
        auto error_tolerances = MRPA::error_tolerance_init(trajectory);

        auto first_tree = init_tree(trajectory, error_tolerances[0], error_tolerances[1]);
        auto first_approx = approximate(trajectory, first_tree, error_tolerances[0]);
        result.emplace_back(first_approx, error_tolerances[0]);

        for (int i = 1; i < error_tolerances.size(); ++i) {
            auto tree = apply_error_tolerance_scale_to_tree(result.back().first, i, error_tolerances);
            auto approximation = approximate(result.back().first, tree, error_tolerances[i]);
            result.emplace_back(approximation, error_tolerances[i]);
        }

        // Remove duplicates that may occur with very small resolution scales
        for (auto i = static_cast<int>(result.size() - 1); i > 0; i--) {
            if (result[i].first.size() == result[i - 1].first.size()) {
                result.erase(std::begin(result) + i);
            }
        }

        return result;

    }

    std::vector<data_structures::Trajectory> MRPA::operator()(Trajectory const& trajectory) const {
        if(resolution_scale > static_cast<double>(trajectory.size())) {
            throw std::invalid_argument("resolution_scale is larger than the trajectory's size");
        }

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

        // Remove duplicates that may occur with very small resolution scales
        for (auto i = static_cast<int>(result.size() - 1); i > 0; i--) {
            if (result[i].size() == result[i - 1].size()) {
                result.erase(std::begin(result) + i);
            }
        }

        return result;
    }


    std::vector<double> MRPA::error_tolerance_init(Trajectory const& trajectory) const {
        std::vector<double> result{};

        auto number_of_tolerances = std::floor(std::log(trajectory.size()) /
                std::log(resolution_scale));

        for (auto k = 1; k <= number_of_tolerances; k++) {
            // Here we add 1 to the resolution and floor it in order to handle cases where the resolution
            // calculation is exactly an int.
            auto resolution = std::floor(static_cast<double>(trajectory.size()) /
                    std::pow(resolution_scale, k) + 1);
            double error_tolerance{};

            for (auto j = 1; j < resolution; j++) {
                auto range_start = static_cast<int>(std::floor((static_cast<double>(trajectory.size() - 1)) /
                        (resolution - 1) * (j - 1) + 1));
                auto range_end = static_cast<int>(std::floor((static_cast<double>(trajectory.size() - 1)) /
                        (resolution - 1) * j + 1));

                if (j == resolution - 1 && range_end < trajectory.size()) {
                    range_end = static_cast<int>(trajectory.size());
                }

                error_tolerance += error_SED_sum(trajectory, range_start, range_end);
            }

            error_tolerance *= 1 / (resolution - 1);
            result.emplace_back(error_tolerance);
        }

        return result;
    }


    double MRPA::error_SED_sum(const Trajectory& trajectory, int i, int j) {

        // Here we subtract one from both i and j, due to 0-indexing
        i = i - 1;
        j = j - 1;
        auto approx_trajectory = approximated_temporally_synchronized_position(trajectory, i, j);
        double res {};
        for (int k = i + 1; k < j; ++k) {
            // Here we index the approx trajectory with the difference of k and i+1.
            // This is because the approx trajectory does not have as many points,
            // therefore it does not have values in the same indexes as the original trajectory.
            res += pow(single_SED(trajectory[k], approx_trajectory[k - (i + 1)]), 2);
        }

        return res;
    }


    data_structures::Trajectory MRPA::approximated_temporally_synchronized_position(Trajectory const& trajectory,
                                                                                    int i, int j) {
        Trajectory res {};

        for (int k = i + 1; k < j; ++k) {
            auto x = static_cast<double>(trajectory[i].longitude +
                     (static_cast<long double>(trajectory[k].timestamp - trajectory[i].timestamp) /
                     static_cast<long double>((trajectory[j].timestamp - trajectory[i].timestamp))) *
                     (trajectory[j].longitude - trajectory[i].longitude));
            auto y = static_cast<double>(trajectory[i].latitude +
                     (static_cast<long double>(trajectory[k].timestamp - trajectory[i].timestamp) /
                     static_cast<long double>((trajectory[j].timestamp - trajectory[i].timestamp))) *
                     (trajectory[j].latitude - trajectory[i].latitude));
            res.locations.emplace_back(trajectory[k].order, trajectory[k].timestamp, x, y);
        }

        return res;
    }


    double MRPA::single_SED(Location const& original_point, Location const& approx_point) {
        return sqrt(pow(original_point.longitude - approx_point.longitude, 2)
        + pow(original_point.latitude - approx_point.latitude, 2));
    }


    data_structures::Node<data_structures::Location> MRPA::init_tree(Trajectory const& trajectory,
                                                             double error_tol, double high_error_tol) {

        MRPA_PTQ working_list{compare};
        working_list.push(trajectory.locations.front());
        MRPA_PTQ future_work(compare);
        std::vector<Location> unvisited{trajectory.locations.begin() + 1, trajectory.locations.end()};

        Node root{working_list.top()};

        while (!unvisited.empty()) {
            while (!working_list.empty()) {
                maintain_priority_queue(root, trajectory, error_tol, high_error_tol, working_list,
                                        future_work, unvisited);
            }

            while (!future_work.empty()) {
                auto fw_point = future_work.top();
                future_work.pop();
                working_list.push(fw_point);
            }
        }

        return root;
    }


    void MRPA::maintain_priority_queue(Node& tree, Trajectory const& trajectory, double error_tol,
                                       double high_error_tol, MRPA_PTQ& working_list, MRPA_PTQ& future_work,
                                       std::vector<Location>& unvisited) {
        Location index1 = working_list.top();
        working_list.pop();

        for (int i = 0; i < unvisited.size(); ++i) {
            const auto index2 = unvisited[i];
            if (index2.order < index1.order) {
                continue;
            }
            auto error = MRPA::error_SED_sum(trajectory, index1.order, index2.order);
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


    data_structures::Trajectory MRPA::approximate(const Trajectory& trajectory, const Node& tree, double error_tol) {

        std::vector<double> approx_error(trajectory.size(), std::numeric_limits<double>::max());
        approx_error[0] = 0;

        std::unordered_map<int, Location > backtrack{}; // Key is point order

        auto parents = std::vector<Node>{tree};
        auto number_output_points = 1;

        while(approx_error[trajectory.locations.back().order - 1] == std::numeric_limits<double>::max()) {
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

        std::deque<Location > result{trajectory.locations.back()};

        for (int i = number_output_points; i >= 2; --i) {
            result.push_front(backtrack[result.front().order]);
        }

        Trajectory result_trajectory{trajectory.id, {std::begin(result), std::end(result)}};

        // Before returning we must update the order values of the nodes.
        // This is because we use order to index, and since we return a trajectory with fewer points,
        // we can index out of range!
        for (int i = 0; i < result_trajectory.size(); ++i) {
            result_trajectory[i].order = i + 1;
        }

        return result_trajectory;
    }


    data_structures::Node<data_structures::Location> MRPA::apply_error_tolerance_scale_to_tree(
            Trajectory const& trajectory, int index, const std::vector<double>& error_tolerances) {

        if (index == error_tolerances.size() - 1) {
            auto scale = error_tolerances[index] / error_tolerances[index - 1];
            return init_tree(trajectory, error_tolerances[index], error_tolerances[index] * scale);
        }
        else {
            return init_tree(trajectory, error_tolerances[index], error_tolerances[index + 1]);
        }
    }

} // simp_algorithms