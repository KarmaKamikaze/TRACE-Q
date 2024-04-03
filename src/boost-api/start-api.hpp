#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <string>
#include "endpoint_handlers.hpp"

namespace api {
// Function to run the server
    void run(boost::asio::ip::tcp::acceptor &acceptor, std::map<std::string, endpoint_handlers::RequestHandler> &endpoints);
}