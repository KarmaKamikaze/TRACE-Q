#include "Endpoint_Handlers.hpp"
#include "boost/json/src.hpp"
#include "boost/json/stream_parser.hpp"
#include "../trajectory_data_handling/Trajectory_Manager.hpp"
#include "../trajectory_data_handling/File_Manager.hpp"

namespace api {
    using namespace boost::beast::http;
    using namespace trajectory_data_handling;

    void add_cors_headers(response<string_body> &res) {
        res.set(field::access_control_allow_origin, "*");  // You can replace * with the specific origin you want to allow
        res.set(field::access_control_allow_methods, "GET, POST, PUT, DELETE, OPTIONS");
        res.set(field::access_control_allow_headers, "Content-Type, Authorization");
    }

    void handle_options(const request<string_body> &req, response<string_body> &res) {
        add_cors_headers(res);
        res.result(status::ok);
        res.set(field::content_type, "text/plain");
        res.body() = "";
    }

    void handle_root(const request<string_body> &req, response<string_body> &res) {
        res.body() = "Welcome to the root endpoint!";
        res.prepare_payload();
        res.set(field::content_type, "text/plain");
        res.result(status::ok);
    }

    boost::json::value get_json_from_request_body(const request<string_body> &req, response<string_body> &res) {
        boost::json::value json_data{};
        try {
            boost::json::error_code ec{};
            json_data = boost::json::parse(req.body(), ec);
            if (ec) throw std::runtime_error("Failed to parse JSON: " + ec.message());
        } catch (const std::exception& e) {
            res.result(status::bad_request);
            res.set(field::content_type, "text/plain");
            res.body() = "Failed to parse JSON: " + std::string(e.what());
            return {};
        }

        return json_data;
    }

    void handle_insert_trajectory_into_trajectory_table(const request<string_body> &req, response<string_body> &res) {
        if (req.method() == verb::options) {
            handle_options(req, res);
            return;
        }

        add_cors_headers(res);
        boost::json::value json_data = get_json_from_request_body(req, res);
        if (json_data.is_null()) {
            return;
        }

        try {
            auto table = json_data.at("table").as_string();
            auto trajectory_id = json_data.at("id").as_string().c_str();

            db_table db_table = (table == "original") ? db_table::original_trajectories
                                                          : db_table::simplified_trajectories;

            auto locations_array = json_data.at("locations").as_array();
            std::vector<data_structures::Location> locations;

            for (auto& location_entry : locations_array) {
                auto timestamp = location_entry.at("timestamp").as_string().c_str();
                auto longitude = location_entry.at("longitude").as_double();
                auto latitude = location_entry.at("latitude").as_double();
                if (trajectory_data_handling::File_Manager::string_to_time(timestamp) == 0)
                    throw std::invalid_argument("Invalid date and time format.");

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
            boost::json::value json_data = get_json_from_request_body(req, res);

            if (!json_data.is_object()) {
                throw std::runtime_error("Invalid JSON data: expected an object");
            }

            const boost::json::object &json_object = json_data.as_object();

            std::string table_key{};
            if (json_object.contains("db_table")) {
                table_key = json_object.at("db_table").as_string().c_str();
            } else {
                throw std::runtime_error("JSON object does not contain 'db_table'");
            }

            db_table db_table{};
            if (table_key == "original") {db_table = db_table::original_trajectories; }
            else if (table_key == "simplified") {db_table = db_table::simplified_trajectories; }
            else { throw std::runtime_error("Error in db_table, must be either 'simplified' or 'original' "); }

            spatial_queries::Range_Query::Window window{};
            const boost::json::object &window_obj = json_object.contains("window") ?
                                                   json_object.at("window").as_object() :
                                                   boost::json::object(); // Empty object if not present

            if (window_obj.contains("x_low")) window.x_low = window_obj.at("x_low").as_double();
            if (window_obj.contains("x_high")) window.x_high = window_obj.at("x_high").as_double();
            if (window_obj.contains("y_low")) window.y_low = window_obj.at("y_low").as_double();
            if (window_obj.contains("y_high")) window.y_high = window_obj.at("y_high").as_double();
            if (window_obj.contains("t_low")) window.t_low = window_obj.at("t_low").as_int64();
            if (window_obj.contains("t_high")) window.t_high = window_obj.at("t_high").as_int64();

            auto ids = spatial_queries::Range_Query::get_ids_from_range_query(
                    trajectory_data_handling::Trajectory_Manager::get_table_name(db_table), window);

            boost::json::array ids_array{};
            for (unsigned int id : ids) {
                ids_array.push_back(id);
            }

            boost::json::object response_object{};
            response_object["trajectory_ids"] = std::move(ids_array);
            res.result(status::ok);
            res.set(field::content_type, "application/json");
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

    void handle_knn_query(const request<string_body> &req, response<string_body> &res){
        try {
            boost::json::value json_data = get_json_from_request_body(req, res);

            if (!json_data.is_object()) {
                throw std::runtime_error("Invalid JSON data: expected an object");
            }

            const boost::json::object &json_object = json_data.as_object();

            std::string db_table{};
            if (json_object.contains("db_table")) {
                const auto& table_value = json_object.at("db_table").as_string();
                if (table_value == "original")
                    db_table = "original_trajectories";
                else if (table_value == "simplified")
                    db_table = "simplified_trajectories";
                else
                    throw std::runtime_error("Error in db_table, must be either 'simplified' or 'original'");
            } else {
                throw std::runtime_error("JSON object does not contain 'db_table'");
            }

            if (!json_object.contains("k")){
                throw std::runtime_error("JSON object does not contain 'k' for KNN");
            }
            int k = json_object.at("k").as_int64();

            spatial_queries::KNN_Query::KNN_Origin query_origin{};
            if (!json_object.contains("query_origin")){
                throw std::runtime_error("JSON object does not contain 'query_origin'");
            }
            const boost::json::object &origin_object = json_object.at("query_origin").as_object();
            query_origin.x = origin_object.at("x").as_double();
            query_origin.y = origin_object.at("y").as_double();
            if (origin_object.contains("t_low")) query_origin.t_low = origin_object.at("t_low").as_int64();
            if (origin_object.contains("t_high")) query_origin.t_high = origin_object.at("t_high").as_int64();

            auto id_dist_pairs = spatial_queries::KNN_Query::get_ids_from_knn(db_table, k, query_origin);

            boost::json::array id_dist_array{};
            for (const auto &pair : id_dist_pairs) {
                boost::json::object result_entry{};
                result_entry["id"] = pair.id;
                result_entry["distance"] = pair.distance;
                id_dist_array.emplace_back(std::move(result_entry));
            }

            boost::json::object response_object{};
            response_object["id_dist_pairs"] = std::move(id_dist_array);
            res.result(status::ok);
            res.set(field::content_type, "application/json");
            std::stringstream ss{};
            ss << response_object;
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
