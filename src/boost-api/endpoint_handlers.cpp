#include "endpoint_handlers.hpp"
#include <utility>
#include <nlohmann/json.hpp>
#include <../data/trajectory_structure.hpp>
#include <../trajectory_data_handling/trajectory_sql.cpp>

using json = nlohmann::json;

namespace api {
    using namespace boost::beast::http;
    using namespace trajectory_data_handling;
    void handle_root(const request<string_body> &req, response<string_body> &res) {
        res.body() = "Welcome to the root endpoint!";
        res.prepare_payload();
        res.set(field::content_type, "text/plain");
        res.result(status::ok);
    }

    void handle_hello(const request<string_body> &req, response<string_body> &res) {
        res.body() = "Hello, world!";
        res.prepare_payload();
        res.set(field::content_type, "text/plain");
        res.result(status::ok);
    }

    void handle_insert_trajectories_into_trajectory_table(const request<string_body> &req, response<string_body> &res) {
        std::string requestBody = req.body();

        json jsonData;
        try {
            jsonData = json::parse(requestBody);
        } catch (const std::exception& e) {
            res.result(status::bad_request);
            res.set(field::content_type, "text/plain");
            res.body() = "Failed to parse JSON: " + std::string(e.what());
            return;
        }

        std::vector<data_structures::Trajectory> all_trajectories{};
        trajectory_data_handling::db_table db_table{};

        for (const auto& table : jsonData.items()) {
            const auto &table_value = table.value();
            db_table = table.key() == "original_trajectories" ? trajectory_data_handling::db_table::original_trajectories : trajectory_data_handling::db_table::simplified_trajectories;

            for(const auto &trajectory_iteration: table_value.items()) {
                const auto &trajectoryData = trajectory_iteration.value();

                data_structures::Trajectory trajectory;
                trajectory.id = std::stoi(trajectory_iteration.key());

                for (const auto &location_iteration: trajectoryData.items()) {
                    data_structures::Location location{};
                    const auto &order = location_iteration.key();
                    const auto &location_data = location_iteration.value();

                    location.order = std::stoi(order);
                    location.timestamp = location_data["timestamp"];
                    location.longitude = location_data["longitude"];
                    location.latitude = location_data["latitude"];

                    trajectory.locations.emplace_back(location);
                }
                all_trajectories.emplace_back(trajectory);
            }
        }
        trajectory_manager::insert_trajectories_into_trajectory_table(all_trajectories, db_table);
    }

    void handle_not_found(const request<string_body> &req, response<string_body> &res) {
        res.result(status::not_found);
        res.body() = "Endpoint not found";
        res.prepare_payload();
    }
}