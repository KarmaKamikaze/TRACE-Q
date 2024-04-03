#pragma once

#include <boost/beast/http.hpp>
#include <map>
#include <functional>

using namespace boost;

// Define a handler type for request handling functions
using RequestHandler = std::function<void(const beast::http::request<beast::http::string_body>&, beast::http::response<beast::http::string_body>&)>;

// Map of endpoints to their corresponding handlers
extern std::map<std::string, RequestHandler> endpoints;

void handle_root(const beast::http::request<beast::http::string_body>& req, beast::http::response<beast::http::string_body>& res);

void handle_hello(const beast::http::request<beast::http::string_body>& req, beast::http::response<beast::http::string_body>& res);

void handle_not_found(const beast::http::request<beast::http::string_body>& req, beast::http::response<beast::http::string_body>& res);

void register_endpoint(const std::string& endpoint, RequestHandler handler);
