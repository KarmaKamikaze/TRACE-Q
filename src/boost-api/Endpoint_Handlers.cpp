#include "Endpoint_Handlers.hpp"
#include "boost/json/src.hpp"
#include "boost/json/stream_parser.hpp"
#include "../trajectory_data_handling/Trajectory_Manager.hpp"
#include "../trajectory_data_handling/File_Manager.hpp"

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

            auto table = jsonData.at("table").as_string();
            auto trajectory_id = jsonData.at("id").as_string().c_str();

            db_table db_table = (table == "original") ? db_table::original_trajectories
                                                          : db_table::simplified_trajectories;

            auto locations_array = jsonData.at("locations").as_array();
            std::vector<data_structures::Location> locations;

            for (auto& location_entry : locations_array) {
                auto timestamp = location_entry.at("timestamp").as_string().c_str();
                auto longitude = location_entry.at("longitude").as_double();
                auto latitude = location_entry.at("latitude").as_double();
                if (trajectory_data_handling::File_Manager::string_to_time(timestamp) == 0){
                    throw std::invalid_argument("Invalid date and time format.");
                }
                auto location = data_structures::Location{0, trajectory_data_handling::File_Manager::string_to_time(timestamp), longitude, latitude};
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
            db_table db_table{};
            if (table_key == "original") {db_table = db_table::original_trajectories; }
            else if (table_key == "simplified") {db_table = db_table::simplified_trajectories; }
            else { throw std::runtime_error("Error in db_table, must be either 'simplified' or 'original' "); }

            // Extract window object or use defaults
            spatial_queries::Range_Query::Window window{};
            const boost::json::object &windowObj = jsonObject.contains("window") ?
                                                   jsonObject.at("window").as_object() :
                                                   boost::json::object(); // Empty object if not present

            if (windowObj.contains("x_low")) { window.x_low = windowObj.at("x_low").as_double(); }
            if (windowObj.contains("x_high")) { window.x_high = windowObj.at("x_high").as_double(); }
            if (windowObj.contains("y_low")) { window.y_low = windowObj.at("y_low").as_double(); }
            if (windowObj.contains("y_high")) { window.y_high = windowObj.at("y_high").as_double(); }
            if (windowObj.contains("t_low")) { window.t_low = windowObj.at("t_low").as_int64(); }
            if (windowObj.contains("t_high")) { window.t_high = windowObj.at("t_high").as_int64(); }

            // Perform the range query and get the IDs
            auto ids = spatial_queries::Range_Query::get_ids_from_range_query(
                    trajectory_data_handling::Trajectory_Manager::get_table_name(db_table), window);

            // Create a JSON array of IDs
            boost::json::array ids_array{};
            for (int id : ids) {
                ids_array.push_back(id);
            }

            // Construct JSON response object
            boost::json::object response_object{};
            response_object["trajectory_ids"] = std::move(ids_array);

            // Set response properties
            res.result(status::ok);
            res.set(field::content_type, "application/json");
            // Serialize JSON response object to string and assign to response body
            std::stringstream ss{};
            ss << response_object; // Serialize JSON object to string
            res.body() = ss.str();
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