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

    void handle_insert_trajectories_into_trajectory_table(const request<string_body> &req, response<string_body> &res) {
        boost::json::value jsonData = get_json_from_request_body(req, res);
        if (jsonData.is_null())
            return;

        std::vector<data_structures::Trajectory> all_trajectories{};
        db_table db_table{};

        try {
            auto jsonObject = jsonData.as_object();
            for (auto & it : jsonObject) {
                const auto &table_key = it.key();
                const auto &table_value = it.value().as_object();
                db_table = table_key == "original" ? db_table::original_trajectories
                                                   : db_table::simplified_trajectories;

                for (const auto & jt : table_value) {
                    const auto &trajectory_id = jt.key();
                    const auto &trajectoryData = jt.value().as_object();

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

                    all_trajectories.emplace_back(std::move(trajectory));
                }
            }

            for (const auto &trajectory : all_trajectories) {
                Trajectory_Manager::insert_trajectory(trajectory, db_table);
            }
            res.result(status::ok);
            res.set(field::content_type, "text/plain");
            res.body() = "Trajectories successfully inserted into the database.";

        } catch (const std::exception &e) {
            res.result(status::bad_request);
            res.set(field::content_type, "text/plain");
            res.body() = "Error processing JSON data: " + std::string(e.what());
        }
    }

    void handle_spatial_range_query_on_rtree_table(const request<string_body> &req, response<string_body> &res) {
        boost::json::value jsonData = get_json_from_request_body(req, res);
        if (jsonData.is_null())
            return;

        try {
            query_purpose purpose;

            if (jsonData.as_object().contains("original")) {
                purpose = query_purpose::load_original_rtree_into_datastructure;
            } else if (jsonData.as_object().contains("simplified")) {
                purpose = query_purpose::load_simplified_rtree_into_datastructure;
            } else {
                res.result(status::bad_request);
                res.set(field::content_type, "text/plain");
                res.body() = "Invalid JSON format: missing 'original' or 'simplified' key";
                return;
            }

            boost::json::object originalData = jsonData.as_object().at("original").as_object();

            double x_low = originalData.at("longitudeRange").as_object().at("min").as_double();
            double x_high = originalData.at("longitudeRange").as_object().at("max").as_double();

            double y_low = originalData.at("latitudeRange").as_object().at("min").as_double();
            double y_high = originalData.at("latitudeRange").as_object().at("max").as_double();

            long double t_low = originalData.at("timestampRange").as_object().at("min").as_double();
            long double t_high = originalData.at("timestampRange").as_object().at("max").as_double();

            spatial_queries::Range_Query::Window window{};
            window.x_low = x_low;
            window.x_high = x_high;
            window.y_low = y_low;
            window.y_high = y_high;
            window.t_low = t_low;
            window.t_high = t_high;

            Trajectory_Manager::spatial_range_query_on_rtree_table(purpose, window);
        } catch (const std::exception &e) {
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