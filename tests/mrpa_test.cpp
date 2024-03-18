#include "../src/simp-algorithms/MRPA.hpp"
#include <doctest/doctest.h>
#include <cmath>

struct test_trajectories {
    data_structures::Trajectory small{};
    data_structures::Trajectory medium{};
    data_structures::Trajectory large{};

    test_trajectories() {
        small.locations.emplace_back(data_structures::Location(1, 0, 1, 2));
        small.locations.emplace_back(data_structures::Location(2, 1, 4, 2));
        small.locations.emplace_back(data_structures::Location(3, 4, 7, 4));
        small.locations.emplace_back(data_structures::Location(4, 12, 17, 9));
        small.locations.emplace_back(data_structures::Location(5, 15, 26, 12));
        small.locations.emplace_back(data_structures::Location(6, 18, 32, 5));

        medium.locations.emplace_back(data_structures::Location(1, 0, 1, 2));
        medium.locations.emplace_back(data_structures::Location(2, 1, 4, 2));
        medium.locations.emplace_back(data_structures::Location(3, 4, 7, 4));
        medium.locations.emplace_back(data_structures::Location(4, 12, 17, 9));
        medium.locations.emplace_back(data_structures::Location(5, 15, 26, 12));
        medium.locations.emplace_back(data_structures::Location(6, 18, 32, 5));
        medium.locations.emplace_back(data_structures::Location(7, 22, 69, 3));
        medium.locations.emplace_back(data_structures::Location(8, 23, 110, 21));
        medium.locations.emplace_back(data_structures::Location(9, 24, 112, 23));

        large.locations.emplace_back(data_structures::Location(1, 0, 1, 2));
        large.locations.emplace_back(data_structures::Location(2, 1, 4, 2));
        large.locations.emplace_back(data_structures::Location(3, 4, 7, 4));
        large.locations.emplace_back(data_structures::Location(4, 12, 17, 9));
        large.locations.emplace_back(data_structures::Location(5, 15, 26, 12));
        large.locations.emplace_back(data_structures::Location(6, 18, 32, 5));
        large.locations.emplace_back(data_structures::Location(7, 22, 69, 3));
        large.locations.emplace_back(data_structures::Location(8, 23, 110, 21));
        large.locations.emplace_back(data_structures::Location(9, 24, 112, 23));
        large.locations.emplace_back(data_structures::Location(10, 28, 143, 28));
        large.locations.emplace_back(data_structures::Location(11, 30, 145, 31));
        large.locations.emplace_back(data_structures::Location(12, 35, 156, 56));
        large.locations.emplace_back(data_structures::Location(13, 37, 169, 61));
        large.locations.emplace_back(data_structures::Location(14, 41, 173, 66));
        large.locations.emplace_back(data_structures::Location(15, 44, 182, 69));
        large.locations.emplace_back(data_structures::Location(16, 50, 243, 98));
        large.locations.emplace_back(data_structures::Location(17, 53, 277, 143));
        large.locations.emplace_back(data_structures::Location(18, 60, 300, 200));
    }
};


// Test if first and last points are correct
// Test if timestamps are increasing
// Test if number of points in the simplification is decreasing


TEST_CASE("MRPA - Correct number of simplifications / throws") {

    auto tt = test_trajectories();
    SUBCASE("c = 2, N = 6") {
        auto mrpa = simp_algorithms::MRPA(2);
        auto res = mrpa(tt.small);

        CHECK((res.size() == std::floor(std::log(6)/std::log(2))));
    }

    SUBCASE("c = 2, N = 9") {
        auto mrpa = simp_algorithms::MRPA(2);
        auto res = mrpa(tt.medium);

        CHECK((res.size() == std::floor(std::log(9)/std::log(2))));
    }

    SUBCASE("c = 2, N = 18") {
        auto mrpa = simp_algorithms::MRPA(2);
        auto res = mrpa(tt.large);

        CHECK((res.size() == std::floor(std::log(18)/std::log(2))));
    }

    SUBCASE("c = 1.1, N = 6") {
        auto mrpa = simp_algorithms::MRPA(1.1);
        auto res = mrpa(tt.small);

        CHECK((res.size() == std::floor(std::log(6)/std::log(1.1))));
    }

    SUBCASE("c = 1.1, N = 9") {
        auto mrpa = simp_algorithms::MRPA(1.1);
        auto res = mrpa(tt.medium);

        CHECK((res.size() == std::floor(std::log(9)/std::log(1.1))));
    }

    SUBCASE("c = 1.1, N = 18") {
        auto mrpa = simp_algorithms::MRPA(1.1);
        auto res = mrpa(tt.large);

        CHECK((res.size() == std::floor(std::log(18)/std::log(1.1))));
    }

    SUBCASE("c = 6, N = 6") {
        auto mrpa = simp_algorithms::MRPA(6);
        auto res = mrpa(tt.small);

        CHECK((res.size() == std::floor(std::log(6)/std::log(6))));
    }

    SUBCASE("c = 6, N = 9") {
        auto mrpa = simp_algorithms::MRPA(6);
        auto res = mrpa(tt.medium);

        CHECK((res.size() == std::floor(std::log(9)/std::log(6))));
    }

    SUBCASE("c = 6, N = 18") {
        auto mrpa = simp_algorithms::MRPA(6);
        auto res = mrpa(tt.large);

        CHECK((res.size() == std::floor(std::log(18)/std::log(6))));
    }

    SUBCASE("c = 10, N = 6") {
        auto mrpa = simp_algorithms::MRPA(10);
        CHECK_THROWS(mrpa(tt.small));
    }
}

TEST_CASE("MRPA - Check if the order sequence corresponds to integers from 1 to N") {
    auto tt = test_trajectories();
    auto mrpa = simp_algorithms::MRPA(2);
    SUBCASE("N = 6"){
        auto res = mrpa(tt.small);

        for (const auto& r : res) {
            for (int i = 0; i < r.size(); ++i) {
                CHECK((r[i].order == i+1));
            }
        }
    }

    SUBCASE("N = 9"){
        auto res = mrpa(tt.medium);

        for (const auto& r : res) {
            for (int i = 0; i < r.size(); ++i) {
                CHECK((r[i].order == i+1));
            }
        }
    }

    SUBCASE("N = 18"){
        auto res = mrpa(tt.large);

        for (const auto& r : res) {
            for (int i = 0; i < r.size(); ++i) {
                CHECK((r[i].order == i+1));
            }
        }
    }



}