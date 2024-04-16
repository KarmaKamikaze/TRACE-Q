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


    data_structures::Trajectory t {};
    t.locations.emplace_back(data_structures::Location(1, 0, 1, 2));
    t.locations.emplace_back(data_structures::Location(2, 1, 4, 2));
    t.locations.emplace_back(data_structures::Location(3, 4, 7, 4));
    t.locations.emplace_back(data_structures::Location(4, 12, 17, 9));
    t.locations.emplace_back(data_structures::Location(5,15, 26, 12));
    t.locations.emplace_back(data_structures::Location(6, 18, 32, 5));
    t.locations.emplace_back(data_structures::Location(7,22, 69, 3));
    t.locations.emplace_back(data_structures::Location(8, 23, 110, 21));
    t.locations.emplace_back(data_structures::Location(9, 24, 112, 23));
    t.locations.emplace_back(data_structures::Location(10, 28, 143, 28));
    t.locations.emplace_back(data_structures::Location(11, 30, 145, 31));
    t.locations.emplace_back(data_structures::Location(12, 35, 156, 56));
    t.locations.emplace_back(data_structures::Location(13, 37, 169, 61));
    t.locations.emplace_back(data_structures::Location(14,41, 173, 66));
    t.locations.emplace_back(data_structures::Location(15, 44, 182, 69));
    t.locations.emplace_back(data_structures::Location(16,50, 243, 98));
    t.locations.emplace_back(data_structures::Location(17, 53, 277, 143));
    t.locations.emplace_back(data_structures::Location(18, 60, 300, 200));

    auto trace_q = trace_q::TRACE_Q{2, 0.1, 3, 1.3, 0.2};
    auto result = trace_q.simplify(t, 0.99);

    for (int i = 0; i < result.locations.size(); ++i) {
        std::cout << "loc" << i + 1 << ", longitude: " << result.locations[i].longitude
        << ", latitude: " << result.locations[i].latitude << ", t: " << result.locations[i].timestamp << '\n';
    }

    // Define and populate endpoints map
    std::map<std::string, api::RequestHandler> endpoints;

    endpoints["/"] = api::handle_root;
    endpoints["/insert"] = api::handle_insert_trajectory_into_trajectory_table;
    endpoints["/db_range_query"] = api::handle_db_range_query;
    // Set up the io_context
    boost::asio::io_context io_context{};
    // Create and bind an acceptor to listen for incoming connections
    boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8080));
    // Run the server
    api::run(acceptor, endpoints);

    return 0;
}
