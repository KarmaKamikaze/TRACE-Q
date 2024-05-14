#include <algorithm>
#include <sstream>
#include <limits>
#include <pqxx/pqxx>
#include "Range_Query.hpp"

namespace spatial_queries {

    std::string Range_Query::connection_string {"user=postgres password=postgres host=localhost dbname=traceq port=5432"};

    bool Range_Query::in_range(data_structures::Trajectory const& trajectory, Window const& window) {
        return std::ranges::any_of(std::cbegin(trajectory.locations), std::cend(trajectory.locations),
                                   [&window](data_structures::Location const& loc){
                                       return loc.longitude >= window.x_low && loc.longitude <= window.x_high
                                              && loc.latitude >= window.y_low && loc.latitude <= window.y_high
                                              && loc.timestamp >= window.t_low && loc.timestamp <= window.t_high;
                                   });
    }

    std::unordered_set<unsigned int> spatial_queries::Range_Query::get_ids_from_range_query(
            std::string const& table, Window const& window) {
        std::stringstream query{};
        query << "SELECT DISTINCT trajectory_id FROM " << table << " WHERE 1=1 ";

        if (window.x_high != std::numeric_limits<double>::max()) {
            query << " AND coordinates[0]<=" << std::to_string(window.x_high);
        }
        if (window.x_low != std::numeric_limits<double>::min()) {
            query << " AND coordinates[0]>=" << std::to_string(window.x_low);
        }
        if (window.y_high != std::numeric_limits<double>::max()) {
            query << " AND coordinates[1]<=" << std::to_string(window.y_high);
        }
        if (window.y_low != std::numeric_limits<double>::min()) {
            query << " AND coordinates[1]>=" << std::to_string(window.y_low);
        }
        if (window.t_high != std::numeric_limits<unsigned long>::max()) {
            query << " AND time<=" << std::to_string(window.t_high);
        }
        if (window.t_low != std::numeric_limits<unsigned long>::min()) {
            query << " AND time>=" << std::to_string(window.t_low);
        }
        query << ";";

        pqxx::connection c{connection_string};
        pqxx::work txn{c};

        auto ids = txn.query<int>(query.str());

        txn.commit();

        std::unordered_set<unsigned int> v_ids{};
        for (const auto& [id] : ids) {
            v_ids.insert(id);
        }

        return v_ids;
    }

} // spatial_queries