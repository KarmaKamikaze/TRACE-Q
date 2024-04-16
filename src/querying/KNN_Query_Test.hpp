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

        KNN_Query_Test(int k, KNN_Query::KNN_Origin const& query_origin)
        : origin{query_origin}, query_result{KNN_Query::knn(table_name, k , query_origin)} {}

        bool operator()(data_structures::Trajectory const& trajectory) override;

        ~KNN_Query_Test() override = default;
    };

} // spatial_queries

#endif //TRACE_Q_KNN_QUERY_TEST_HPP
