#include <pqxx/pqxx>
#include <cmath>
#include <future>
#include <algorithm>
#include "Benchmark.hpp"
#include "../querying/Range_Query.hpp"


namespace analytics {

    std::string Benchmark::connection_string{"user=postgres password=postgres host=localhost dbname=traceq port=5432"};

    double Benchmark::grid_density{0.2};
    double Benchmark::expansion_factor{grid_density * 0.8};
    int Benchmark::windows_per_grid_point{3};
    double Benchmark::window_expansion_rate{1.3};
    double Benchmark::time_interval{0.2};
    int Benchmark::knn_k{5};
    int Benchmark::max_connections{50};

    MBR Benchmark::get_mbr() {

        auto query = "SELECT min(coordinates[0]) as min_x, max(coordinates[0]) as max_x, min(coordinates[1]) as min_y, "
                     "max(coordinates[1]) as max_y, min(time) as min_t, max(time) as max_t FROM original_trajectories;";

        pqxx::connection c{connection_string};

        pqxx::work txn{c};

        auto result = txn.exec1(query);
        txn.commit();

        MBR mbr{};

        mbr.x_low = result["min_x"].as<double>() / (1 + expansion_factor);
        mbr.x_high = result["max_x"].as<double>() * (1 + expansion_factor);
        mbr.y_low = result["min_y"].as<double>() / (1 + expansion_factor);
        mbr.y_high = result["max_y"].as<double>() * (1 + expansion_factor);
        mbr.t_low = result["min_t"].as<unsigned long>();
        mbr.t_high = result["max_t"].as<unsigned long>();

        return mbr;
    }

    Query_Accuracy Benchmark::benchmark_query_accuracy() {

        auto points_on_axis = static_cast<int>(std::ceil(1 / grid_density));
        auto time_points = static_cast<int>(std::ceil(1 / time_interval));

        auto mbr = get_mbr();

        std::vector<std::future<F1>> range_futures{};
        std::vector<std::future<F1>> knn_futures{};

        std::vector<F1> range_query_cum_F1{};
        std::vector<F1> knn_query_cum_F1{};
        int query_count = 0;

        for (int x_point = 0; x_point <= points_on_axis; x_point++) {
            auto x_coord = mbr.x_low + x_point * grid_density * (mbr.x_high - mbr.x_low);
            for (int y_point = 0; y_point <= points_on_axis; y_point++) {
                auto y_coord = mbr.y_low + y_point * grid_density * (mbr.y_high - mbr.y_low);
                for (int t_point = 0; t_point <= time_points; ++t_point) {
                    auto t_interval = static_cast<unsigned long>(std::round(mbr.t_low + t_point
                            * time_interval * static_cast<long double>(mbr.t_high - mbr.t_low)));

                    for (auto rq_futures = create_range_query_futures(
                            x_coord, y_coord, t_interval, mbr); auto& fut : rq_futures) {
                        query_count++;
                        range_futures.emplace_back(std::move(fut));
                    }

                    knn_futures.emplace_back(create_knn_query_future(knn_k, x_coord, y_coord, t_interval, mbr));
                    query_count++;

                    if (query_count > max_connections) {
                        for (auto & fut : range_futures) {
                            range_query_cum_F1.push_back(fut.get());
                        }
                        for (auto & fut : knn_futures) {
                            knn_query_cum_F1.push_back(fut.get());
                        }
                        range_futures.clear();
                        knn_futures.clear();
                        query_count = 0;
                    }
                }
                knn_futures.emplace_back(create_knn_query_future(knn_k, x_coord, y_coord, 0, mbr));
                query_count++;
            }
        }

        for (auto & fut : range_futures) {
            range_query_cum_F1.push_back(fut.get());
        }
        for (auto & fut : knn_futures) {
            knn_query_cum_F1.push_back(fut.get());
        }

        auto range_query_f1 = F1::calc_cum_F1_score(range_query_cum_F1);
        auto knn_query_f1 = F1::calc_cum_F1_score(knn_query_cum_F1);
        return Query_Accuracy{range_query_f1, knn_query_f1};
    }

    std::vector<std::future<F1>> Benchmark::create_range_query_futures(double x, double y, unsigned long t, MBR const& mbr) {
        std::vector<std::future<F1>> futures;
        for(int window_number = 0; window_number < windows_per_grid_point; window_number++) {
            auto [window_x_low, window_x_high] = calculate_window_range(
                    x, mbr.x_low, mbr.x_high, window_expansion_rate, grid_density, window_number);
            auto [window_y_low, window_y_high] = calculate_window_range(
                    y, mbr.y_low, mbr.y_high, window_expansion_rate, grid_density, window_number);
            auto [window_t_low, window_t_high] = calculate_time_range(
                    t, mbr.t_low, mbr.t_high, window_expansion_rate,
                    time_interval, window_number);

            spatial_queries::Range_Query::Window window{window_x_low, window_x_high, window_y_low,
                                                        window_y_high, window_t_low, window_t_high};
            futures.emplace_back(std::async(std::launch::async, [window](){
                auto original_res = spatial_queries::Range_Query::get_ids_from_range_query("original_trajectories", window);
                auto simplified_res = spatial_queries::Range_Query::get_ids_from_range_query("simplified_trajectories", window);

                return F1(original_res, simplified_res);
            }));
        }

        return futures;
    }

    std::future<F1> Benchmark::create_knn_query_future(int k, double x, double y, unsigned long t, const MBR & mbr) {

        spatial_queries::KNN_Query::KNN_Origin origin{};
        origin.x = x;
        origin.y = y;
        if (t != 0) {
            auto [t_low, t_high] = calculate_time_range(
                    t, mbr.t_low, mbr.t_high, 0,
                    time_interval, 0);

            origin.t_low = t_low;
            origin.t_high = t_high;
        }
        else {
            origin.t_low = std::numeric_limits<unsigned long>::min();
            origin.t_high = std::numeric_limits<unsigned long>::max();
        }

        return std::future<F1>{std::async(std::launch::async, [origin, k](){
            auto original_res = spatial_queries::KNN_Query::get_ids_from_knn("original_trajectories", k, origin);
            auto simplified_res = spatial_queries::KNN_Query::get_ids_from_knn("simplified_trajectories", k, origin);

            return F1(original_res, simplified_res);
        })};
    }

    std::pair<double, double> Benchmark::calculate_window_range(
            double center, double mbr_low, double mbr_high, double w_expansion_rate,
            double w_grid_density, int window_number) {
        auto w_low = center - 0.5 * (pow(w_expansion_rate, window_number)
                                     * w_grid_density * (mbr_high - mbr_low));
        if (w_low < mbr_low) {
            w_low = mbr_low;
        }
        auto w_high = center + 0.5 * (pow(w_expansion_rate, window_number)
                                      * w_grid_density * (mbr_high - mbr_low));
        if (w_high > mbr_high) {
            w_high = mbr_high;
        }
        return {w_low, w_high};
    }

    std::pair<unsigned long, unsigned long> Benchmark::calculate_time_range(
            unsigned long center, unsigned long mbr_low, unsigned long mbr_high,
            double w_expansion_rate, double w_grid_density, int window_number) {
        auto w_low = static_cast<unsigned long>(std::floor(center - 0.5 * (pow(w_expansion_rate, window_number)
                                                                           * w_grid_density * static_cast<long double>(mbr_high - mbr_low))));
        if (w_low < mbr_low) {
            w_low = mbr_low;
        }
        auto w_high = static_cast<unsigned long>(std::ceil(center + 0.5 * (pow(w_expansion_rate, window_number)
                                                                           * w_grid_density * static_cast<long double>(mbr_high - mbr_low))));
        if (w_high > mbr_high) {
            w_high = mbr_high;
        }
        return {w_low, w_high};
    }

}


