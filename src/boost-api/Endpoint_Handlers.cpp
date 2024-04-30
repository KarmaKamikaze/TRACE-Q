#include "Endpoint_Handlers.hpp"
#include "boost/json/src.hpp"
#include "boost/json/stream_parser.hpp"
#include "../trajectory_data_handling/Trajectory_Manager.hpp"
#include "../trajectory_data_handling/File_Manager.hpp"
#include "TRACE_Q.hpp"

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
            if (ec)
                throw std::runtime_error("Failed to parse JSON: " + ec.message());
        } catch (const std::exception& e) {
            res.result(status::bad_request);
            res.set(field::content_type, "text/plain");
            res.body() = "Failed to parse JSON: " + std::string(e.what());
            return {};
        }

        return json_data;
    }

    boost::json::object convert_trajectory_to_json(const std::vector<data_structures::Trajectory>& trajectory){
        boost::json::object json_traj{};
        json_traj["id"] = trajectory[0].id;

        boost::json::array json_locations{};
        for (const auto &loc: trajectory[0].locations) {
            boost::json::object json_location{};
            json_location["order"] = loc.order;
            json_location["timestamp"] = loc.timestamp;
            json_location["longitude"] = loc.longitude;
            json_location["latitude"] = loc.latitude;
            json_locations.push_back(std::move(json_location));
        }

        json_traj["locations"] = std::move(json_locations);

        return json_traj;
    }

    /**
         This endpoint handles insertion of a trajectory into a specified database table. Works with JSON formatted as:

         {
            "db_table": "original",
            "id": "10000",
            "locations": [
                {
                    "timestamp": "2008-02-02 13:33:52",
                    "longitude": 1.0,
                    "latitude": 2.0
                },
                {
                    "timestamp": "2008-03-02 13:33:52",
                    "longitude": 2.0,
                    "latitude": 3.0
                }
            ]
        }
    */
    void handle_insert_trajectory_into_trajectory_table(const request<string_body> &req, response<string_body> &res) {
        try {
            if (req.method() == verb::options) {
                handle_options(req, res);
                return;
            }
            add_cors_headers(res);

            boost::json::value json_data = get_json_from_request_body(req, res);
            if (json_data.is_null())
                throw std::runtime_error("No JSON data was provided.");

            auto trajectory_id = json_data.at("id").as_string().c_str();
            const boost::json::object &json_object = json_data.as_object();
            std::string table_key{};

            if (json_object.contains("db_table"))
                table_key = json_object.at("db_table").as_string().c_str();
            else
                throw std::runtime_error("JSON object does not contain 'db_table'.");

            db_table db_table{};
            if (table_key == "original")
                db_table = db_table::original_trajectories;
            else if (table_key == "simplified")
                db_table = db_table::simplified_trajectories;
            else
                throw std::runtime_error("Error in db_table, must be either 'simplified' or 'original'.");

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

    /**
         This endpoint handles range queries on a specified database table. Works with JSON formatted as:

         {
            "db_table": "original",
            "window" : {
                "x_low"  : 0.0,
                "x_high" : 10.0,
                "y_low"  : 0.0,
                "y_high" : 30.0,
                "t_low"  : 0,
                "t_high" : 0
            }
        }

        and (option without time):

        {
            "db_table": "original",
            "window" : {
                "x_low"  : 0.0,
                "x_high" : 10.0,
                "y_low"  : 0.0,
                "y_high" : 30.0
            }
        }
    */
    void handle_db_range_query(const request<string_body> &req, response<string_body> &res) {
        try {
            if (req.method() == verb::options) {
                handle_options(req, res);
                return;
            }
            add_cors_headers(res);

            boost::json::value json_data = get_json_from_request_body(req, res);

            if (!json_data.is_object())
                throw std::runtime_error("Invalid JSON data: expected an object");

            const boost::json::object &json_object = json_data.as_object();

            std::string table_key{};
            if (json_object.contains("db_table"))
                table_key = json_object.at("db_table").as_string().c_str();
            else
                throw std::runtime_error("JSON object does not contain 'db_table'");

            db_table db_table{};
            if (table_key == "original")
                db_table = db_table::original_trajectories;
            else if (table_key == "simplified")
                db_table = db_table::simplified_trajectories;
            else
                throw std::runtime_error("Error in db_table, must be either 'simplified' or 'original' ");

            spatial_queries::Range_Query::Window window{};
            const boost::json::object &window_obj = json_object.contains("window") ?
                                                   json_object.at("window").as_object() :
                                                   boost::json::object(); // Empty object if not present

            if (window_obj.contains("x_low"))
                window.x_low = window_obj.at("x_low").as_double();
            if (window_obj.contains("x_high"))
                window.x_high = window_obj.at("x_high").as_double();
            if (window_obj.contains("y_low"))
                window.y_low = window_obj.at("y_low").as_double();
            if (window_obj.contains("y_high"))
                window.y_high = window_obj.at("y_high").as_double();
            if (window_obj.contains("t_low"))
                window.t_low = window_obj.at("t_low").as_int64();
            if (window_obj.contains("t_high"))
                window.t_high = window_obj.at("t_high").as_int64();

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

    /**
         This endpoint handles knn queries on a specified database table. Works with JSON formatted as:

         {
            "db_table": "original",
            "k" : 2,
            "query_origin": {
                "x" : 20.0,
                "y"  : 20.0,
                "t_low" : 10000,
                "t_high" : 100000
            }
        }

        and (option without time):

        {
            "db_table": "original",
            "k" : 2,
            "query_origin": {
                "x" : 20.0,
                "y"  : 20.0
            }
        }

    */
    void handle_knn_query(const request<string_body> &req, response<string_body> &res){
        try {
            if (req.method() == verb::options) {
                handle_options(req, res);
                return;
            }
            add_cors_headers(res);

            boost::json::value json_data = get_json_from_request_body(req, res);

            if (!json_data.is_object())
                throw std::runtime_error("Invalid JSON data: expected an object");


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

            if (!json_object.contains("k"))
                throw std::runtime_error("JSON object does not contain 'k' for KNN");

            int k = json_object.at("k").as_int64();

            spatial_queries::KNN_Query::KNN_Origin query_origin{};
            if (!json_object.contains("query_origin"))
                throw std::runtime_error("JSON object does not contain 'query_origin'");

            const boost::json::object &origin_object = json_object.at("query_origin").as_object();
            query_origin.x = origin_object.at("x").as_double();
            query_origin.y = origin_object.at("y").as_double();
            if (origin_object.contains("t_low"))
                query_origin.t_low = origin_object.at("t_low").as_int64();
            if (origin_object.contains("t_high"))
                query_origin.t_high = origin_object.at("t_high").as_int64();

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

    /**
         This endpoint loads a trajectory from a specified id and database table. Works with JSON formatted as:

         {
            "db_table": "original",
            "id" : 1
        }
    */
    void handle_load_trajectory_from_id (const request<string_body> &req, response<string_body> &res) {
        try {
            if (req.method() == verb::options) {
                handle_options(req, res);
                return;
            }
            add_cors_headers(res);

            boost::json::value json_data = get_json_from_request_body(req, res);

            if (!json_data.is_object())
                throw std::runtime_error("Invalid JSON data: expected an object");

            const boost::json::object &json_object = json_data.as_object();

            std::string table_key{};
            if (json_object.contains("db_table"))
                table_key = json_object.at("db_table").as_string().c_str();
            else
                throw std::runtime_error("JSON object does not contain 'db_table'");

            db_table db_table{};
            if (table_key == "original")
                db_table = db_table::original_trajectories;
            else if (table_key == "simplified")
                db_table = db_table::simplified_trajectories;
            else
                throw std::runtime_error("Error in db_table, must be either 'simplified' or 'original' ");

            if (!json_object.contains("id"))
                throw std::runtime_error("JSON object does not contain 'id'");

            std::vector<unsigned int> id{static_cast<unsigned int>(json_object.at("id").as_int64())};

            std::vector<data_structures::Trajectory> trajectory = Trajectory_Manager::load_into_data_structure(db_table,id);
            boost::json::object response_trajectory{convert_trajectory_to_json(trajectory)};

            boost::json::object response_object{};
            response_object["response_trajectory"] = std::move(response_trajectory);
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
    /**
         This endpoint removes all data from all database tables
    */
    void handle_reset_all_data(const request<string_body> &req, response<string_body> &res){
        try{
            if (req.method() == verb::options) {
                handle_options(req, res);
                return;
            }
            add_cors_headers(res);

            trajectory_data_handling::Trajectory_Manager::reset_all_data();
            res.result(status::ok);
            res.set(field::content_type, "text/plain");
            res.body() = "All data has successfully been removed from the database";
        }
        catch (const std::exception &e){
            res.result(status::bad_request);
            res.set(field::content_type, "text/plain");
            res.body() = "An error occurred while attempting to remove all data from the database: " + std::string(e.what());
        }
    }

    /**
         This endpoint performs simplification on the database. Works with JSON formatted as:

         {
            "resolution_scale" : 2.0,
            "min_query_accuracy" : 0.95,
            "range_query_grid_density": 0.1,
            "knn_query_grid_density": 0.1,
            "windows_per_grid_point": 3,
            "window_expansion_rate" : 1.3,
            "range_query_time_interval" : 0.2,
            "knn_query_time_interval" : 0.03,
            "knn_k" : 10
        }

    */
    void handle_run_simplification(const request<string_body> &req, response<string_body> &res) {
        try {
            if (req.method() == verb::options) {
                handle_options(req, res);
                return;
            }
            add_cors_headers(res);

            boost::json::value json_data = boost::json::parse(req.body());

            if (!json_data.is_object())
                throw std::runtime_error("Invalid JSON data: expected an object");

            const boost::json::object &json_object = json_data.as_object();

            trace_q::TRACE_Q trace_q{
                json_object.at("resolution_scale").as_double(),
                json_object.at("min_query_accuracy").as_double(),
                json_object.at("range_query_grid_density").as_double(),
                json_object.at("knn_query_grid_density").as_double(),
                static_cast<int>(json_object.at("windows_per_grid_point").as_int64()),
                json_object.at("window_expansion_rate").as_double(),
                json_object.at("range_query_time_interval").as_double(),
                json_object.at("knn_query_time_interval").as_double(),
                static_cast<int>(json_object.at("knn_k").as_int64())
            };

            trace_q.run();

            res.result(boost::beast::http::status::ok);
            res.set(boost::beast::http::field::content_type, "text/plain");
            res.body() = "Simplification process completed successfully";
        }
        catch (const std::exception &e) {
            res.result(boost::beast::http::status::bad_request);
            res.set(boost::beast::http::field::content_type, "text/plain");
            res.body() = "Error processing JSON data: " + std::string(e.what());
        }
    }
    /**
     This endpoint checks that there are the same amount of trajectories in both the original_trajectories and simplified_trajectories databases. No body is needed for this endpoint.
     */
    void handle_db_status(const request<string_body> &req, response<string_body> &res) {
        try {
            bool db_status = trajectory_data_handling::Trajectory_Manager::get_db_status();

            res.result(boost::beast::http::status::ok);
            res.set(boost::beast::http::field::content_type, "text/plain");
            res.body() = db_status ? "true" : "false";
        } catch(const std::exception &e) {
            res.result(boost::beast::http::status::bad_request);
            res.set(boost::beast::http::field::content_type, "text/plain");
            res.body() = "Error in handle_db_status: " + std::string(e.what());
        }
    }

    /**
     This endpoint finds trajectories from both the original_trajectories and simplified_trajectories tables from an id and a date. It works with JSON such as:
     {
        "id" : 1,
        "date" : "2008-02-02"
    }
     */
    void handle_load_trajectories_from_id_and_time(const request<string_body> &req, response<string_body> &res){
        try {
            if (req.method() == verb::options) {
                handle_options(req, res);
                return;
            }
            add_cors_headers(res);

            boost::json::value json_data = boost::json::parse(req.body());
            const boost::json::object &json_object = json_data.as_object();

            if (!json_data.is_object())
                throw std::runtime_error("Invalid JSON data: expected an object");

            if (!json_object.contains("id"))
                throw std::runtime_error("JSON object does not contain 'id'");

            if (!json_object.contains("date"))
                throw std::runtime_error("JSON object does not contain 'date'");

            auto date{json_object.at("date").as_string().c_str()};
            auto id {json_object.at("id").as_int64()};

            auto original_trajectory = Trajectory_Manager::get_trajectory_from_id_table_date(id, trajectory_data_handling::db_table::original_trajectories, date);
            auto simplified_trajectory = Trajectory_Manager::get_trajectory_from_id_table_date(id, trajectory_data_handling::db_table::simplified_trajectories, date);
            boost::json::object json_original_trajectory{convert_trajectory_to_json(original_trajectory)};
            boost::json::object json_simplified_trajectory{convert_trajectory_to_json(simplified_trajectory)};

            boost::json::object response_object{};
            response_object["original_trajectory"] = std::move(json_original_trajectory);
            response_object["simplified_trajectory"] = std::move(json_simplified_trajectory);
            res.result(status::ok);
            res.set(field::content_type, "application/json");
            std::stringstream ss{};
            ss << response_object;
            res.body() = ss.str();
        }
        catch (const std::exception &e){
            res.result(boost::beast::http::status::bad_request);
            res.set(boost::beast::http::field::content_type, "text/plain");
            res.body() = "Error processing JSON data: " + std::string(e.what());
        }
    }

    void handle_get_dates_from_id(const request<string_body> &req, response<string_body> &res) {
        try {
            if (req.method() == verb::options) {
                handle_options(req, res);
                return;
            }
            add_cors_headers(res);

            boost::json::value json_data = boost::json::parse(req.body());
            const boost::json::object &json_object = json_data.as_object();

            if (!json_data.is_object())
                throw std::runtime_error("Invalid JSON data: expected an object");

            if (!json_object.contains("id"))
                throw std::runtime_error("JSON object does not contain 'id'");

            auto id {json_object.at("id").as_int64()};

            auto dates = Trajectory_Manager::get_dates_from_id(id);

            boost::json::object response_object{};
            boost::json::array json_dates{};
            for (auto & date : dates) {
                json_dates.emplace_back(date);
            }
            response_object["dates"] = std::move(json_dates);
            res.result(status::ok);
            res.set(field::content_type, "application/json");
            res.body() = boost::json::serialize(response_object);
        }
        catch (const std::exception &e){
            res.result(boost::beast::http::status::bad_request);
            res.set(boost::beast::http::field::content_type, "text/plain");
            res.body() = "Error processing JSON data: " + std::string(e.what());
        }
    }
    

    void handle_not_found(const request<string_body> &req, response<string_body> &res) {
        res.result(status::not_found);
        res.body() = "Endpoint not found";
        res.prepare_payload();
    }
}
