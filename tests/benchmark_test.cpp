#include "../src/benchmark/benchmark.hpp"
#include <doctest/doctest.h>

void time_func(auto t) {
    _sleep(t);
}


TEST_CASE("test see how far benchmark time is from 600 ms") {
    auto t = 600;

    std::cout << "testing " << t << " ms : \n";
    {
        benchmark benchmark;
        time_func(t);
    }


}

TEST_CASE("test see how far benchmark time is from 1000 ms") {
    auto t = 1000;

    std::cout << "testing " << t << " ms : \n";
    {
        benchmark benchmark;
        time_func(t);
    }

}

TEST_CASE("test see how far benchmark time is from 10000 ms") {
    auto t = 10000;

    std::cout << "testing " << t << " ms : \n";
    {
        benchmark benchmark;
        time_func(t);
    }

}

TEST_CASE("test see how far benchmark time is from 100000 ms") {
    auto t = 100000;

    std::cout << "testing " << t << " ms : \n";
    {
        benchmark benchmark;
        time_func(t);
    }

}
