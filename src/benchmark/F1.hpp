#ifndef TRACE_Q_F1_HPP
#define TRACE_Q_F1_HPP

#include <unordered_set>
#include "../querying/KNN_Query.hpp"

namespace analytics {

    class F1 {
    private:
        int true_positives{};
        int false_positives{};
        int false_negatives{};

    public:

        F1(std::vector<spatial_queries::KNN_Query::KNN_Result_Element> const& original_vec,
           std::vector<spatial_queries::KNN_Query::KNN_Result_Element> const& simplified_vec);

        F1(std::unordered_set<unsigned int> const& original_set, std::unordered_set<unsigned int> const& simplified_set);

        static double calc_cum_F1_score(std::vector<F1> const& f1_values);

    };

}

#endif //TRACE_Q_F1_HPP
