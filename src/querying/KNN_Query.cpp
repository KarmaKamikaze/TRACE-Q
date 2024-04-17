#include <pqxx/pqxx>
#include <iostream>
#include <sstream>
#include <limits>
#include "KNN_Query.hpp"

namespace spatial_queries {

    std::string KNN_Query::connection_string {"user=postgres password=postgres host=localhost dbname=traceq port=5432"};

    std::vector<KNN_Query::KNN_Result_Element> KNN_Query::get_ids_from_knn(
            std::string const& table, int k, KNN_Origin const& query_origin) {
        pqxx::connection c{connection_string};
        pqxx::work txn{c};
        std::stringstream query{};

        query << "SELECT trajectory_id, MIN(coordinates <-> POINT("
              << std::to_string(query_origin.x) << "," << std::to_string(query_origin.y)
              << ")) AS dist FROM " << table;
        if (query_origin.t_low != std::numeric_limits<unsigned long>::min()
            && query_origin.t_high != std::numeric_limits<unsigned long>::max()) {
            query << " WHERE time >= " << query_origin.t_low << " AND time <= " << query_origin.t_high;
        }
        else if (query_origin.t_low != std::numeric_limits<unsigned long>::min()) {
            query << " WHERE time >= " << query_origin.t_low;
        }
        else if (query_origin.t_high != std::numeric_limits<unsigned long>::max()) {
            query << " WHERE time <= " << query_origin.t_high;
        }
        query << " GROUP BY trajectory_id ORDER BY MIN(coordinates <-> POINT("
              << std::to_string(query_origin.x) << "," << std::to_string(query_origin.y) << ")) LIMIT " << k << ";";

        auto query_result = txn.query<int, double>(query.str());

        txn.commit();
        std::vector<KNN_Result_Element> result{};
        for (auto& [id, dist] : query_result) {
            result.emplace_back(id, dist);
        }

        return result;
    }

} // spatial_queries