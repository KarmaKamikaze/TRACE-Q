#include "Endpoint_Handlers.hpp"
#include "boost/json/src.hpp"
#include "boost/json/stream_parser.hpp"
#include "../trajectory_data_handling/Trajectory_Manager.hpp"

namespace api {
    using namespace boost::beast::http;
    using namespace trajectory_data_handling;

    void handle_root(const request<string_body> &req, response<string_body> &res) {
        res.body() = "Welcome to the root endpoint!";
        res.prepare_payload();
        res.set(field::content_type, "text/plain");
        res.result(status::ok);
    }

    boost::json::value get_json_from_request_body(const request<string_body> &req, response<string_body> &res) {
        boost::json::value jsonData{};
        try {
            boost::json::error_code ec{};
            jsonData = boost::json::parse(req.body(), ec);
            if (ec)
                throw std::runtime_error("Failed to parse JSON: " + ec.message());
        } catch (const std::exception& e) {
            res.result(status::bad_request);
            res.set(field::content_type, "text/plain");
            res.body() = "Failed to parse JSON: " + std::string(e.what());
            return {};
        }

        return jsonData;
    }

    void handle_insert_trajectory_into_trajectory_table(const request<string_body> &req, response<string_body> &res) {
        boost::json::value jsonData = get_json_from_request_body(req, res);
        if (jsonData.is_null()) {
            return;
        }

        try {
            auto jsonObject = jsonData.as_object();

            if (jsonObject.size() != 1) {
                throw std::runtime_error("JSON must contain exactly one trajectory object.");
            }

            const auto &table_key = jsonObject.begin()->key();
            const auto &table_value = jsonObject.begin()->value().as_object();

            db_table db_table = (table_key == "original") ? db_table::original_trajectories
                                                          : db_table::simplified_trajectories;

            if (table_value.size() != 1) {
                throw std::runtime_error("JSON must contain exactly one trajectory under the specified key.");
            }

            const auto &trajectory_id = table_value.begin()->key();
            const auto &trajectoryData = table_value.begin()->value().as_object();

            std::vector<data_structures::Location> locations{};

            for (const auto &kt : trajectoryData) {
                const auto &location_order = kt.key();
                const auto &location_data = kt.value().as_object();

                data_structures::Location location{};
                location.order = std::stoi(location_order);
                location.timestamp = location_data.at("timestamp").as_int64();
                location.longitude = location_data.at("longitude").as_double();
                location.latitude = location_data.at("latitude").as_double();
                locations.emplace_back(location);
            }

            unsigned int id = static_cast<unsigned int>(std::stoi(trajectory_id));
            data_structures::Trajectory trajectory{id, std::move(locations)};

            Trajectory_Manager::insert_trajectory(trajectory, db_table);

            res.result(status::ok);
            res.set(field::content_type, "text/plain");
            res.body() = "Trajectory successfully inserted into the database.";

        } catch (const std::exception &e) {
            res.result(status::bad_request);
            res.set(field::content_type, "text/plain");
            res.body() = "Error processing JSON data: " + std::string(e.what());
        }
    }

    void handle_db_range_query(const request<string_body> &req, response<string_body> &res) {
        try {
            boost::json::value jsonData = get_json_from_request_body(req, res);

            // Check if the JSON data is an object
            if (!jsonData.is_object()) {
                throw std::runtime_error("Invalid JSON data: expected an object");
            }

            const boost::json::object &jsonObject = jsonData.as_object();

            // Extract db_table value
            std::string table_key;
            if (jsonObject.contains("db_table")) {
                table_key = jsonObject.at("db_table").as_string().c_str();
            } else {
                throw std::runtime_error("JSON object does not contain 'db_table'");
            }

            // Determine the db_table enum based on the table_key
            db_table db_table = (table_key == "original") ? db_table::original_trajectories
                                                               : db_table::simplified_trajectories;

            // Extract window object or use defaults
            spatial_queries::Range_Query::Window window;
            const boost::json::object &windowObj = jsonObject.contains("window") ?
                                                   jsonObject.at("window").as_object() :
                                                   boost::json::object(); // Empty object if not present

            // Assign values from JSON or use defaults
            window.x_low = windowObj.contains("x_low") ? windowObj.at("x_low").as_double() : std::numeric_limits<double>::lowest();
            window.x_high = windowObj.contains("x_high") ? windowObj.at("x_high").as_double() : std::numeric_limits<double>::max();
            window.y_low = windowObj.contains("y_low") ? windowObj.at("y_low").as_double() : std::numeric_limits<double>::lowest();
            window.y_high = windowObj.contains("y_high") ? windowObj.at("y_high").as_double() : std::numeric_limits<double>::max();
            window.t_low = windowObj.contains("t_low") ? windowObj.at("t_low").as_int64() : std::numeric_limits<long>::lowest();
            window.t_high = windowObj.contains("t_high") ? windowObj.at("t_high").as_int64() : std::numeric_limits<long>::max();

            // Perform the db_range_query with extracted values
            trajectory_data_handling::Trajectory_Manager::db_range_query(db_table, window);

            // Respond with success if no exceptions were thrown
            res.result(status::ok);
            res.set(field::content_type, "text/plain");
            res.body() = "Query executed successfully";
        }
        catch (const std::exception &e) {
            res.result(status::bad_request);
            res.set(field::content_type, "text/plain");
            res.body() = "Error processing JSON data: " + std::string(e.what());
        }
    }


    void handle_not_found(const request<string_body> &req, response<string_body> &res) {
        res.result(status::not_found);
        res.body() = "Endpoint not found";
        res.prepare_payload();
    }
}