#pragma once

#include <boost/beast/http.hpp>
#include <map>
#include <functional>

namespace api {
    using namespace boost::beast::http;

    using RequestHandler = std::function<void(const request<string_body> &, response<string_body> &)>;

    void handle_insert_trajectory_into_trajectory_table(const request<string_body> &req, response<string_body> &res);

    void handle_spatial_range_query_on_rtree_table(const request<string_body> &req, response<string_body> &res);

    void handle_root(const request<string_body> &req, response<string_body> &res);

    void handle_not_found(const request<string_body> &req, response<string_body> &res);
}