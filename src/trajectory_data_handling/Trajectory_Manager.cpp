#include <iostream>
#include <fstream>
#include <pqxx/pqxx>
#include "Trajectory_Manager.hpp"
#include "File_Manager.hpp"

namespace trajectory_data_handling {

    std::string Trajectory_Manager::connection_string{"user=postgres password=postgres host=localhost dbname=traceq port=5432"};

    void Trajectory_Manager::insert_trajectory(data_structures::Trajectory const& trajectory, db_table table) {
        auto table_name = get_table_name(table);

        pqxx::connection c{connection_string};
        pqxx::work txn{c};

        std::stringstream first_query{};
        first_query << "INSERT INTO " << table_name << "(trajectory_id, coordinates, time) " << " VALUES("
                    << trajectory.id << ", point(" << std::to_string(trajectory[0].longitude) << ", " << std::to_string(trajectory[0].latitude) << "), "
                    << std::to_string(trajectory[0].timestamp) << ");";

        txn.exec0(first_query.str());

        if (table == db_table::original_trajectories) {
            for (int i = 1; i < trajectory.size(); i++) {
                if (trajectory[i].timestamp != trajectory[i - 1].timestamp) {
                    std::stringstream query{};
                    query << "INSERT INTO " << table_name << "(trajectory_id, coordinates, time) " << " VALUES("
                          << trajectory.id << ", point(" << std::to_string(trajectory[i].longitude) << ", "
                          << std::to_string(trajectory[i].latitude) << "), "
                          << std::to_string(trajectory[i].timestamp) << ");";

                    txn.exec0(query.str());
                }
            }
        }
        else if (table == db_table::simplified_trajectories) {
            for (int i = 1; i < trajectory.size(); i++) {
                std::stringstream query{};
                query << "INSERT INTO " << table_name << "(trajectory_id, coordinates, time) " << " VALUES("
                      << trajectory.id << ", point(" << std::to_string(trajectory[i].longitude) << ", "
                      << std::to_string(trajectory[i].latitude) << "), "
                      << std::to_string(trajectory[i].timestamp) << ");";

                txn.exec0(query.str());
            }
        }

        txn.commit();
    }

     std::vector<data_structures::Trajectory> Trajectory_Manager::load_into_data_structure(
             db_table table, std::vector<unsigned int> const& ids) {
        auto table_name = get_table_name(table);
        std::stringstream query{};

        if(ids.empty()) {
            query << "SELECT trajectory_id, coordinates, time FROM " << table_name << " ORDER BY trajectory_id;";
        }
        else {
            query << "SELECT trajectory_id, coordinates, time FROM " << table_name << " WHERE trajectory_id IN (";
            for(auto i = ids.cbegin(); i != ids.cend(); ++i) {
                query << *i;
                if (std::next(i) != ids.end()) {
                    query << ",";
                }
            }
            query << ") ORDER BY trajectory_id;";
        }

        pqxx::connection c{connection_string};
        pqxx::work txn{c};
        pqxx::result query_res{txn.exec(query.str())};
        txn.commit();
        std::vector<data_structures::Trajectory> res{};

        data_structures::Trajectory working_trajectory{};
        auto current_trajectory_id = query_res.front()["trajectory_id"].as<int>();
        working_trajectory.id = current_trajectory_id;
        auto next_order = 1;

        for (auto const& row : query_res) {
            auto trajectory_id = row["trajectory_id"].as<int>();
            auto coords = row["coordinates"].c_str();
            auto time = row["time"].as<long>();

            if(current_trajectory_id != trajectory_id) {
                res.push_back(working_trajectory);
                working_trajectory = data_structures::Trajectory{};
                working_trajectory.id = trajectory_id;
                current_trajectory_id = trajectory_id;
                next_order = 1;
            }

            auto location = parse_location(next_order, time, coords);
            working_trajectory.locations.push_back(location);
            next_order++;
        }

        res.push_back(working_trajectory);
        return res;
    }

    data_structures::Location Trajectory_Manager::parse_location(int order, unsigned long time, std::string const& coordinates) {
        auto coords = std::string{coordinates};

        std::erase(coords, '(');
        std::erase(coords, ')');

        std::istringstream iss(coords);
        std::string lat_str{};
        std::string lon_str{};
        getline(iss, lon_str, ',');
        getline(iss, lat_str, ',');

        return data_structures::Location{order, time, std::stod(lon_str), std::stod(lat_str)};
    }

    void Trajectory_Manager::print_trajectories(std::vector<data_structures::Trajectory> const& all_trajectories) {
        for (const auto & trajectory : all_trajectories) {
            std::cout << "id: " << trajectory.id << std::endl;
            for (const auto &location: trajectory.locations) {
                std::cout << "order: " << location.order << '\n';
                std::cout << "timestamp: " <<  location.timestamp << '\n';
                std::cout << "longitude, latitude: " << location.longitude << " " << location.latitude << '\n';
            }
        }
    }

    void Trajectory_Manager::create_database() {
        pqxx::connection c{connection_string};
        pqxx::work txn{c};

        add_query_file_to_transaction("../../sql/create_table_original.sql", txn);
        add_query_file_to_transaction("../../sql/create_table_simplified.sql", txn);

        txn.commit();
    }

    void Trajectory_Manager::create_simplified_database() {
        pqxx::connection c{connection_string};
        pqxx::work txn{c};

        add_query_file_to_transaction("../../sql/create_table_simplified.sql", txn);

        txn.commit();
    }

    void Trajectory_Manager::reset_all_data() {
        pqxx::connection c{connection_string};
        pqxx::work txn{c};
        txn.exec0("DROP INDEX IF EXISTS original_trajectories_index;");
        txn.exec0("DROP TABLE IF EXISTS original_trajectories;");
        txn.exec0("DROP INDEX IF EXISTS simplified_trajectories_index;");
        txn.exec0("DROP TABLE IF EXISTS simplified_trajectories;");

        txn.commit();
        create_database();
    }

    void Trajectory_Manager::reset_simplified_data() {
        pqxx::connection c{connection_string};
        pqxx::work txn{c};
        txn.exec0("DROP INDEX IF EXISTS simplified_trajectories_index;");
        txn.exec0("DROP TABLE IF EXISTS simplified_trajectories;");

        txn.commit();
        create_simplified_database();
    }

    void Trajectory_Manager::add_query_file_to_transaction(std::string const& query_file_path,
                                                           pqxx::work &transaction) {
        std::ifstream ifs{query_file_path};
        std::string query(std::istreambuf_iterator<char>{ifs}, {});
        transaction.exec0(query);
    }

    std::string Trajectory_Manager::get_table_name(db_table table) {
        switch(table) {
            case db_table::original_trajectories:
                return "original_trajectories";
            case db_table::simplified_trajectories:
                return "simplified_trajectories";
            default:
                throw std::invalid_argument("Error in switch statement in get_table_name");
        }
    }

    std::vector<unsigned int> Trajectory_Manager::db_get_all_trajectory_ids(trajectory_data_handling::db_table table) {
        auto table_name = get_table_name(table);

        std::stringstream query{};

        query << "SELECT DISTINCT trajectory_id FROM " << table_name << ";";

        pqxx::connection c{connection_string};
        pqxx::work txn{c};

        auto query_result = txn.query<int>(query.str());
        txn.commit();

        auto result = std::vector<unsigned int>{};
        for (auto& [id] : query_result) {
            result.emplace_back(id);
        }

        return result;
    }

    bool Trajectory_Manager::get_db_status() {
        std::stringstream query{};

        pqxx::connection c{connection_string};
        pqxx::work txn{c};

        query << "SELECT COUNT(DISTINCT trajectory_id) FROM original_trajectories;";
        int original_count = txn.exec1(query.str())[0].as<int>();

        query.str(std::string());
        query << "SELECT COUNT(DISTINCT trajectory_id) FROM simplified_trajectories;";
        int simplified_count = txn.exec1(query.str())[0].as<int>();

        txn.commit();

        return (original_count != 0 && simplified_count != 0 && original_count == simplified_count);
    }

    std::vector<data_structures::Trajectory> Trajectory_Manager::get_trajectory_from_id_table_date(int trajectory_id, trajectory_data_handling::db_table table, const std::string& date){
        auto table_name = get_table_name(table);
        std::stringstream datestream{};
        pqxx::connection c{connection_string};
        pqxx::work txn{c};

        datestream << date << " 00:00:00";
        auto date_start = File_Manager::string_to_time(datestream.str());

        datestream.str(std::string());

        datestream << date << " 23:59:59";
        auto date_end = File_Manager::string_to_time(datestream.str());

        std::stringstream query{};

        query << "SELECT trajectory_id, coordinates, time FROM " << table_name << " WHERE trajectory_id = " << trajectory_id
              << " AND time >= '" << date_start << "' AND time <= '" << date_end << "';";

        pqxx::result query_res{txn.exec(query.str())};
        txn.commit();
        std::vector<data_structures::Trajectory> res{};

        data_structures::Trajectory working_trajectory{};
        auto current_trajectory_id = query_res.front()["trajectory_id"];
        if (current_trajectory_id.is_null())
            throw std::runtime_error("Trajectory ID is null or not found in the database. Consider running simplification first!");
        working_trajectory.id = current_trajectory_id.as<int>();
        auto next_order = 1;

        for (auto const& row : query_res) {
            auto trajectory_id = row["trajectory_id"].as<int>();
            auto coords = row["coordinates"].c_str();
            auto time = row["time"].as<long>();

            if (current_trajectory_id.as<int>() != trajectory_id) {
                res.push_back(working_trajectory);
                working_trajectory = data_structures::Trajectory{};
                current_trajectory_id = row["trajectory_id"];
                working_trajectory.id = trajectory_id;
                next_order = 1;
            }

            auto location = parse_location(next_order, time, coords);
            working_trajectory.locations.push_back(location);
            next_order++;
        }

        res.push_back(working_trajectory);
        return res;
    }

    std::vector<std::string> Trajectory_Manager::get_dates_from_id(int trajectory_id){
        std::stringstream datestream{};
        pqxx::connection c{connection_string};
        pqxx::work txn{c};

        std::stringstream query{};
        query << "SELECT DISTINCT to_char(to_timestamp(time), 'YYYY-MM-DD') as date FROM original_trajectories WHERE trajectory_id =" << trajectory_id << ";";

        pqxx::result query_res{txn.exec(query.str())};
        txn.commit();
        std::vector<std::string> res{};

        for (auto const& row : query_res) {
            res.emplace_back(row["date"].c_str());
        }
        return res;
    }
}







