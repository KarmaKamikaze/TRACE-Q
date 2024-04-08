#include "endpoint_handlers.hpp"
#include <utility>
#include <nlohmann/json.hpp>
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

    json get_json_from_request_body(const request<string_body> &req,  response<string_body> &res) {
        std::string requestBody = req.body();

        json jsonData{};
        try {
            jsonData = json::parse(requestBody);
        } catch (const std::exception& e) {
            res.result(status::bad_request);
            res.set(field::content_type, "text/plain");
            res.body() = "Failed to parse JSON: " + std::string(e.what());
        }

        return jsonData;
    }

    void handle_insert_trajectories_into_trajectory_table(const request<string_body> &req, response<string_body> &res) {
        json jsonData = get_json_from_request_body(req, res);
        if (jsonData.empty())
            return;

        std::vector<data_structures::Trajectory> all_trajectories{};
        trajectory_data_handling::db_table db_table{};

        for (const auto& table : jsonData.items()) {
            const auto &table_value = table.value();
            db_table = table.key() == "original" ? trajectory_data_handling::db_table::original_trajectories : trajectory_data_handling::db_table::simplified_trajectories;

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

    void handle_load_trajectories_into_rtree(const request<string_body> &req, response<string_body> &res) {
        json jsonData = get_json_from_request_body(req, res);
        if (jsonData.empty())
            return;

        std::string db_table;
        try {
            db_table = jsonData.at("db_table").get<std::string>();
        } catch (const std::exception& e) {
            res.result(status::bad_request);
            res.set(field::content_type, "text/plain");
            res.body() = "Failed to read 'db_table' value from JSON: " + std::string(e.what());
            return;
        }

        trajectory_data_handling::query_purpose purpose = db_table == "original" ? trajectory_data_handling::query_purpose::insert_into_original_rtree_table : trajectory_data_handling::query_purpose::insert_into_simplified_rtree_table;

        trajectory_data_handling::trajectory_manager::load_trajectories_into_rtree(purpose);
    }

    void handle_spatial_range_query_on_rtree_table(const request<string_body> &req, response<string_body> &res) {
        json jsonData = get_json_from_request_body(req, res);
        if (jsonData.empty())
            return;

        trajectory_data_handling::query_purpose purpose;
        if (jsonData.contains("original")) {
            purpose = trajectory_data_handling::query_purpose::load_original_rtree_into_datastructure;
        } else if (jsonData.contains("simplified")) {
            purpose = trajectory_data_handling::query_purpose::load_simplified_rtree_into_datastructure;
        } else {
            res.result(status::bad_request);
            res.set(field::content_type, "text/plain");
            res.body() = "Invalid JSON format: missing 'original' or 'simplified' key";
            return;
        }

        auto& longitudeRange = jsonData["original"]["longitudeRange"];
        double x_low = longitudeRange["min"];
        double x_high = longitudeRange["max"];

        auto& latitudeRange = jsonData["original"]["latitudeRange"];
        double y_low = latitudeRange["min"];
        double y_high = latitudeRange["max"];

        auto& timestampRange = jsonData["original"]["timestampRange"];
        long double t_low = timestampRange["min"];
        long double t_high = timestampRange["max"];

        spatial_queries::Range_Query::Window window{};
        window.x_low = x_low;
        window.x_high = x_high;
        window.y_low = y_low;
        window.y_high = y_high;
        window.t_low = t_low;
        window.t_high = t_high;

        trajectory_data_handling::trajectory_manager::spatial_range_query_on_rtree_table(purpose, window);
    }

    void handle_reset_all_data(const request<string_body> &req, response<string_body> &res) {
        trajectory_data_handling::trajectory_manager::reset_all_data();
    }

    void handle_not_found(const request<string_body> &req, response<string_body> &res) {
        res.result(status::not_found);
        res.body() = "Endpoint not found";
        res.prepare_payload();
    }
}