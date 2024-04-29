#include "F1.hpp"

namespace analytics {

    F1::F1(std::vector<spatial_queries::KNN_Query::KNN_Result_Element> const& original_vec,
    std::vector<spatial_queries::KNN_Query::KNN_Result_Element> const& simplified_vec) {
        std::unordered_set<unsigned int> simplified_set{};

        for (const auto& s : simplified_vec) {
            simplified_set.insert(s.id);
        }

        for (const auto& original : original_vec) {
            if (simplified_set.contains(original.id)) {
                true_positives++;
            }
        }

        false_positives = static_cast<int>(original_vec.size()) - true_positives;
        false_negatives = false_positives;
    }

    F1::F1(std::unordered_set<unsigned int> const& original_set, std::unordered_set<unsigned int> const& simplified_set) {

        for (const auto& original : original_set) {
            if(simplified_set.contains(original)) {
                true_positives++;
            }
        }

        false_negatives = static_cast<int>(original_set.size()) - true_positives;
    }

    double F1::calc_cum_F1_score(std::vector<F1> const& f1_values) {
        int total_true_positives{};
        int total_false_positives{};
        int total_false_negatives{};

        for (const auto& f1_v : f1_values) {
            total_true_positives += f1_v.true_positives;
            total_false_positives += f1_v.false_positives;
            total_false_negatives += f1_v.false_negatives;
        }

        return total_true_positives / (total_true_positives + 0.5 * (total_false_positives + total_false_negatives));
    }

}
