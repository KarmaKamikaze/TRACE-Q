#pragma once

#include <boost/beast/http.hpp>
#include <map>
#include <functional>
#include "../trajectory_data_handling/trajectory_sql.hpp"

namespace api {
    using namespace boost::beast::http;

    // Define a handler type for request handling functions
    using RequestHandler = std::function<void(const request<string_body> &, response<string_body> &)>;

    void handle_insert_trajectories_into_trajectory_table(const request<string_body> &req, response<string_body> &res);

    void handle_spatial_range_query_on_rtree_table(const request<string_body> &req, response<string_body> &res);

    void handle_root(const request<string_body> &req, response<string_body> &res);

    void handle_hello(const request<string_body> &req, response<string_body> &res);

    void handle_not_found(const request<string_body> &req, response<string_body> &res);
}