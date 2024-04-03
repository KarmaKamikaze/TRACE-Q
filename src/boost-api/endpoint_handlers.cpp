#include "endpoint_handlers.hpp"
#include <utility>

using namespace boost;

// Map of endpoints to their corresponding handlers
std::map<std::string, RequestHandler> endpoints;

void handle_root(const beast::http::request<beast::http::string_body>& req, beast::http::response<beast::http::string_body>& res) {
    res.body() = "Welcome to the root endpoint!";
    res.prepare_payload();
    res.set(beast::http::field::content_type, "text/plain");
    res.result(beast::http::status::ok);
}

void handle_hello(const beast::http::request<beast::http::string_body>& req, beast::http::response<beast::http::string_body>& res) {
    res.body() = "Hello, world!";
    res.prepare_payload();
    res.set(beast::http::field::content_type, "text/plain");
    res.result(beast::http::status::ok);
}

void handle_not_found(const beast::http::request<beast::http::string_body>& req, beast::http::response<beast::http::string_body>& res) {
    res.result(beast::http::status::not_found);
    res.body() = "Endpoint not found";
    res.prepare_payload();
}

void register_endpoint(const std::string& endpoint, RequestHandler handler) {
    endpoints[endpoint] = std::move(handler);
}
