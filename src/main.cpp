#include <iostream>
#include "trajectory_data_handling/Trajectory_Manager.hpp"
#include "trajectory_data_handling/File_Manager.hpp"
#include "TRACE_Q.hpp"
#include <boost/asio.hpp>
#include "Start_API.hpp"
#include "Endpoint_Handlers.hpp"

int main(int argc, char* argv[]) {
    
    for(int i = 1; i < argc; i++) {
        if(argv[i] == std::string{"--reset"}) {
            trajectory_data_handling::Trajectory_Manager::reset_all_data();
        }
    }

    // TRACE-Q test
    trajectory_data_handling::Trajectory_Manager::reset_all_data();
    trajectory_data_handling::File_Manager::load_tdrive_dataset();

    auto trace_q = trace_q::TRACE_Q{2, 0.95, 0.1, 0.1, 3, 1.3, 0.2, 0.03, 10};
    trace_q.run();


    // Define and populate endpoints map
    std::map<std::string, api::RequestHandler> endpoints;

    endpoints["/"] = api::handle_root;
    endpoints["/insert"] = api::handle_insert_trajectory_into_trajectory_table;
    endpoints["/query"] = api::handle_spatial_range_query_on_rtree_table;

    // Set up the io_context
    boost::asio::io_context io_context{};
    // Create and bind an acceptor to listen for incoming connections
    boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8080));
    // Run the server
    api::run(acceptor, endpoints);

    return 0;
}
