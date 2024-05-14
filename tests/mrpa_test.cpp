#include "../src/simp-algorithms/MRPA.hpp"
#include "test_trajectories.hpp"
#include <doctest/doctest.h>
#include <cmath>
#include <iostream>


TEST_CASE("MRPA - Check if the order sequence corresponds to integers from 1 to M") {
    auto tt = test_trajectories();
    auto mrpa = simp_algorithms::MRPA(2);
    SUBCASE("N = 6"){
        auto res = mrpa(tt.small);

        for (const auto& r : res) {
            for (int i = 0; i < r.size(); ++i) {
                CHECK(r[i].order == i+1);
            }
        }
    }

    SUBCASE("N = 9"){
        auto res = mrpa(tt.medium);

        for (const auto& r : res) {
            for (int i = 0; i < r.size(); ++i) {
                CHECK(r[i].order == i+1);
            }
        }
    }

    SUBCASE("N = 18"){
        auto res = mrpa(tt.large);

        for (const auto& r : res) {
            for (int i = 0; i < r.size(); ++i) {
                CHECK(r[i].order == i+1);
            }
        }
    }
}

TEST_CASE("MRPA - Check if timestamps are increasing") {
    auto tt = test_trajectories();
    auto mrpa = simp_algorithms::MRPA(1.2);

    SUBCASE("N = 6"){
        auto res = mrpa(tt.small);

        for (const auto& r : res) {
            long double prev_timestamp = -1;
            for (const auto& l : r.locations) {
                CHECK(l.timestamp > prev_timestamp);
                prev_timestamp = l.timestamp;
            }
        }
    }

    SUBCASE("N = 9"){
        auto res = mrpa(tt.medium);

        for (const auto& r : res) {
            long double prev_timestamp = -1;
            for (const auto& l : r.locations) {
                CHECK(l.timestamp > prev_timestamp);
                prev_timestamp = l.timestamp;
            }
        }
    }

    SUBCASE("N = 18"){
        auto res = mrpa(tt.large);

        for (const auto& r : res) {
            long double prev_timestamp = -1;
            for (const auto& l : r.locations) {
                CHECK(l.timestamp > prev_timestamp);
                prev_timestamp = l.timestamp;
            }
        }
    }
}

TEST_CASE("MRPA - Check if resolution of simplifications is decreasing") {
    auto tt = test_trajectories();
    auto mrpa = simp_algorithms::MRPA(1.2);

    SUBCASE("N = 6") {
        auto res = mrpa(tt.small);

        size_t prev_num_points = 7;
        for (const auto& r : res) {
            CHECK(r.size() < prev_num_points);
            prev_num_points = r.size();
        }
    }

    SUBCASE("N = 9") {
        auto res = mrpa(tt.medium);

        size_t prev_num_points = 10;
        for (const auto& r : res) {
            CHECK(r.size() < prev_num_points);
            prev_num_points = r.size();
        }
    }

    SUBCASE("N = 18") {
        auto res = mrpa(tt.large);

        size_t prev_num_points = 19;
        for (const auto& r : res) {
            CHECK(r.size() < prev_num_points);
            prev_num_points = r.size();
        }
    }
}

TEST_CASE("MRPA - Check if the first and last locations in simplifications are correct") {
    auto tt = test_trajectories();
    auto mrpa = simp_algorithms::MRPA(1.2);

    SUBCASE("N = 6"){
        auto first = tt.small.locations.front();
        auto last = tt.small.locations.back();
        auto res = mrpa(tt.small);

        for (const auto& r : res) {
            CHECK(tt.compare_locations(r.locations.front(), first));
            CHECK(tt.compare_locations(r.locations.back(), last));
        }
    }

    SUBCASE("N = 9"){
        auto first = tt.medium.locations.front();
        auto last = tt.medium.locations.back();
        auto res = mrpa(tt.medium);

        for (const auto& r : res) {
            CHECK(tt.compare_locations(r.locations.front(), first));
            CHECK(tt.compare_locations(r.locations.back(), last));
        }
    }

    SUBCASE("N = 18"){
        auto first = tt.large.locations.front();
        auto last = tt.large.locations.back();
        auto res = mrpa(tt.large);

        for (const auto& r : res) {
            CHECK(tt.compare_locations(r.locations.front(), first));
            CHECK(tt.compare_locations(r.locations.back(), last));
        }
    }
}

TEST_CASE("MRPA - Simplification of a straight line yields only the first and last locations") {
    auto straight_trajectory = data_structures::Trajectory{};
    straight_trajectory.locations.emplace_back(data_structures::Location(1, 0, 0, 0));
    straight_trajectory.locations.emplace_back(data_structures::Location(2, 1, 1, 1));
    straight_trajectory.locations.emplace_back(data_structures::Location(3, 2, 2, 2));
    straight_trajectory.locations.emplace_back(data_structures::Location(4, 3, 3, 3));
    straight_trajectory.locations.emplace_back(data_structures::Location(5, 4, 4, 4));
    straight_trajectory.locations.emplace_back(data_structures::Location(6, 5, 5, 5));

    auto tt = test_trajectories{};
    auto mrpa = simp_algorithms::MRPA(1.2);

    SUBCASE("Straight or not"){
        auto res = mrpa(straight_trajectory);
        auto first = straight_trajectory.locations.front();
        auto last = straight_trajectory.locations.back();

        CHECK(res.size() == 1);
        CHECK(res.front().size() == 2);
        CHECK(tt.compare_locations(res.front().locations.front(), first));
        CHECK(tt.compare_locations(res.front().locations.back(), last));
    }
}
