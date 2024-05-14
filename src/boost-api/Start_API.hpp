#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <string>
#include "Endpoint_Handlers.hpp"

namespace api {
// Function to run the server
    void run(boost::asio::ip::tcp::acceptor &acceptor, std::map<std::string, api::RequestHandler> &endpoints);
}