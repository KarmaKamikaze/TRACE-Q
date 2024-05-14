#include <iostream>
#include <boost/asio.hpp>
#include "trajectory_data_handling/Trajectory_Manager.hpp"
#include "trajectory_data_handling/File_Manager.hpp"
#include "TRACE_Q.hpp"
#include "Start_API.hpp"
#include "Endpoint_Handlers.hpp"
#include "benchmark/TRACE_Q_Benchmark.hpp"

int main(int argc, char* argv[]) {
    
    for(int i = 1; i < argc; i++) {
        if (argv[i] == std::string{"--reset"}) {
            trajectory_data_handling::Trajectory_Manager::reset_all_data();
        }
        if (argv[i] == std::string("--benchmark")) {
            analytics::TRACE_Q_Benchmark::run_traceq_benchmarks(200);
            analytics::TRACE_Q_Benchmark::run_traceq_vs_mrpa(500);
            return 0;
        }
    }

    // Define and populate endpoints map
    std::map<std::string, api::RequestHandler> endpoints;

    endpoints["/"] = api::handle_root;
    endpoints["/insert"] = api::handle_insert_trajectory_into_trajectory_table;
    endpoints["/db_range_query"] = api::handle_db_range_query;
    endpoints["/knn_query"] = api::handle_knn_query;
    endpoints["/load_from_id"] = api::handle_load_trajectory_from_id;
    endpoints["/reset"] = api::handle_reset_all_data;
    endpoints["/run"] = api::handle_run_simplification;
    endpoints["/status"] = api::handle_db_status;
    endpoints["/load_from_id_date"] = api::handle_load_trajectories_from_id_and_time;
    endpoints["/get_dates_from_id"] = api::handle_get_dates_from_id;
    // Set up the io_context
    boost::asio::io_context io_context{};
    // Create and bind an acceptor to listen for incoming connections
    boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8080));
    // Run the server
    std::cout << "The server is now running and you are ready to perform API calls." << std::endl;
    api::run(acceptor, endpoints);

    return 0;
}
