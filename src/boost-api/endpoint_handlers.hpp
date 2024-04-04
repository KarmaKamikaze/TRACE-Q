#pragma once

#include <boost/beast/http.hpp>
#include <map>
#include <functional>

using namespace boost::beast::http;

namespace api {
    // Define a handler type for request handling functions
    using RequestHandler = std::function<void(const request<string_body> &, response<string_body> &)>;

    void handle_root(const request<string_body> &req, response<string_body> &res);

    void handle_hello(const request<string_body> &req, response<string_body> &res);

    void handle_not_found(const request<string_body> &req, response<string_body> &res);
}