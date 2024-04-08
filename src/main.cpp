#include <iostream>
#include "trajectory_data_handling/trajectory_sql.hpp"
#include "trajectory_data_handling/sqlite_querying.hpp"
#include "trajectory_data_handling/trajectory_file_manager.hpp"
#include "TRACE_Q.hpp"
#include <boost/asio.hpp>
#include "start-api.hpp"
#include "endpoint_handlers.hpp"

int main() {
    trajectory_data_handling::file_manager file_manager{};
    auto original_trajectories = std::make_shared<std::vector<data_structures::Trajectory>>();
    trajectory_data_handling::query_handler::original_trajectories = original_trajectories;

    auto simplified_trajectories = std::make_shared<std::vector<data_structures::Trajectory>>();
    trajectory_data_handling::query_handler::simplified_trajectories = simplified_trajectories;

    trajectory_data_handling::trajectory_manager trajectory_manager{};


//    trajectory_manager.reset_all_data();
//    trajectory_manager.create_database();
//    trajectory_manager.create_rtree_table();
//    file_manager.load_tdrive_dataset(*original_trajectories);
//    file_manager.load_geolife_dataset(*original_trajectories);
//    trajectory_manager.insert_trajectories_into_trajectory_table(*original_trajectories, trajectory_data_handling::db_table::original_trajectories);
//    trajectory_manager.load_database_into_datastructure(trajectory_data_handling::query_purpose::load_original_trajectory_information_into_datastructure);
//    trajectory_manager.insert_trajectories_into_trajectory_table(*original_trajectories, trajectory_data_handling::db_table::simplified_trajectories);

//    std::tuple<float, float> longitudeRange, latitudeRange, timestampRange;
//    longitudeRange = std::make_tuple(115, 116.65901184082031);
//    latitudeRange = std::make_tuple(40.07930374145508, 41);
//    timestampRange = std::make_tuple(0, 91202490113);
//
//    trajectory_manager.spatial_range_query_on_rtree_table(trajectory_data_handling::query_purpose::load_original_rtree_into_datastructure, longitudeRange, latitudeRange, timestampRange);
//
//
//    trajectory_manager.print_trajectories(*original_trajectories);
//    trajectory_manager.load_trajectories_into_rtree(trajectory_data_handling::query_purpose::insert_into_original_rtree_table);
//    trajectory_manager.load_trajectories_into_simplified_rtree();
//    trajectory_manager.reset_all_data();

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

//    for (int i = 0; i < result.locations.size(); ++i) {
//        std::cout << "loc" << i + 1 << ", longitude: " << result.locations[i].longitude
//        << ", latitude: " << result.locations[i].latitude << ", t: " << result.locations[i].timestamp << '\n';
//    }

    // Define and populate endpoints map
    std::map<std::string, api::RequestHandler> endpoints;

    endpoints["/"] = api::handle_root;
    endpoints["/hello"] = api::handle_hello;
    endpoints["/insert"] = api::handle_insert_trajectories_into_trajectory_table;
    endpoints["/query"] = api::handle_spatial_range_query_on_rtree_table;

    // Set up the io_context
    boost::asio::io_context io_context{};
    // Create and bind an acceptor to listen for incoming connections
    boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8080));
    // Run the server
    api::run(acceptor, endpoints);

    return 0;
}
