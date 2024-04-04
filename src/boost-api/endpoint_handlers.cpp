#include "endpoint_handlers.hpp"
#include <utility>

using namespace boost::beast::http;

namespace api {
    // Map of endpoints to their corresponding handlers
    std::map<std::string, RequestHandler> endpoints;

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

    void handle_not_found(const request<string_body> &req, response<string_body> &res) {
        res.result(status::not_found);
        res.body() = "Endpoint not found";
        res.prepare_payload();
    }

    void register_endpoint(const std::string &endpoint, RequestHandler handler) {
        endpoints[endpoint] = std::move(handler);
    }
}