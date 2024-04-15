#ifndef TRACE_Q_KNN_QUERY_HPP
#define TRACE_Q_KNN_QUERY_HPP

#include <pqxx/pqxx>
#include <vector>
#include <string>

namespace spatial_queries {

    struct KNN_Query {
        /**
         * An element from the result of performing a KNN query.
         */
        struct KNN_Result_Element {
            unsigned int id{};
            double distance{};
        };

        /**
         * The origin point used in K-Nearest-Neighbour (KNN) queries.
         */
        struct KNN_Origin {
            double x{};
            double y{};
            long double t_low{};
            long double t_high{};
        };

        /**
         * Performs a K-Nearest-Neighbour query on the given database given an origin point.
         * @param table The table to query.
         * @param k The amount of nearest neighbours to return.
         * @param query_origin The origin point from where the nearest neighbours are discovered using euclidean distance.
         * @return A list of KNN Results corresponding to the K-Nearest-Neighbours from the origin point. The result
         * element consists of a trajectory ID and a distance from the origin point.
         */
        static std::vector<KNN_Result_Element> knn(std::string const& table, int k, KNN_Origin const& query_origin);

    private:
        /**
         * The connection string that specifies the connection details for the PostgreSQL database.
         */
        static std::string connection_string;
    };

} // spatial_queries

#endif //TRACE_Q_KNN_QUERY_HPP
