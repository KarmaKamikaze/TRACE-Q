#ifndef TRACE_Q_KNN_QUERY_TEST_HPP
#define TRACE_Q_KNN_QUERY_TEST_HPP

#include "Query.hpp"
#include "KNN_Query.hpp"

namespace spatial_queries {

    class KNN_Query_Test : public Query {

        /**
         * The origin point for the K-Nearest-Neighbour (KNN) query.
         */
        KNN_Query::KNN_Origin origin{};

        std::vector<KNN_Query::KNN_Result_Element> query_result{};

        /**
         * The name of the table to perform KNN queries on.
         */
        static std::string table_name;

        /**
         * Calculates the euclidean distance between a location and an origin point used in KNN queries.
         * @param location A location in a trajectory.
         * @param origin A origin point for a KNN query.
         * @return The euclidean distance between the two points.
         */
        static double euclidean_distance(data_structures::Location const& location, KNN_Query::KNN_Origin const& origin);

    public:

        /**
         * A boolean that describes if the original trajectory is contained within the K-Nearest-Neighbour (KNN) query.
         * This is used to discard the True Negatives query results.
         */
        bool original_in_result{false};

        KNN_Query_Test(int original_trajectory_id, int k, KNN_Query::KNN_Origin const& query_origin)
        : origin{query_origin}, query_result{KNN_Query::get_ids_from_knn(table_name, k + 1, query_origin)} {
            for (int i = 0; i < k; i++) {
                if (original_trajectory_id == query_result[i].id) {
                    original_in_result = true;
                }
            }
        }

        bool operator()(data_structures::Trajectory const& trajectory) override;

        ~KNN_Query_Test() override = default;
    };

} // spatial_queries

#endif //TRACE_Q_KNN_QUERY_TEST_HPP
