#include "Start_API.hpp"

namespace api {
    // Function to run the server
    void run(boost::asio::ip::tcp::acceptor &acceptor, std::map<std::string, api::RequestHandler> &endpoints) {
        // Set up the io_context
        boost::asio::io_context io_context{};

        // Accept incoming connections in a loop
        while (true) {
            // Create a socket
            boost::asio::ip::tcp::socket socket(io_context);

            // Wait for and accept a connection
            acceptor.accept(socket);

            // Declare the error_code variable here
            boost::system::error_code ec;

            while (true) {
                // Read the HTTP request
                boost::beast::flat_buffer buffer{};
                boost::beast::http::request<boost::beast::http::string_body> request{};
                boost::beast::http::read(socket, buffer, request, ec);

                if (ec) {
                    break;  // Break the inner loop if error occurs (e.g., connection closed by client)
                }

                // Prepare an HTTP response
                boost::beast::http::response<boost::beast::http::string_body> response{};

                // Find and call the handler for the requested endpoint
                auto it = endpoints.find(request.target());
                if (it != endpoints.end()) {
                    it->second(request, response);
                } else {
                    api::handle_not_found(request, response);
                }

                // Send the response
                boost::beast::http::write(socket, response, ec);
                if (ec || response.need_eof()) {
                    break;  // Break the inner loop if error occurs or if it's the last response
                }
            }

            // Gracefully close the socket
            socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
            socket.close(ec);
        }
    }
}
