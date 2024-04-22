#include "../src/benchmark/Benchmark.hpp"
#include <doctest/doctest.h>
#include <iostream>

void time_func(int t) {
    _sleep(t);
}


TEST_CASE("test see if benchmark time is above a specified time") {

    SUBCASE("600 ms")
        int t;
        auto t = 600;
        auto tp = t + t * 0.05;
        auto duration = analytics::function_time(time_func, t);
        CHECK((t <= duration && duration < tp));

    SUBCASE("1000 ms")
        int s;
        auto s = 1000;
        auto sp = s + s * 0.05;
        auto durs = analytics::function_time(time_func, s);
        CHECK((s <= durs && durs < sp));

}

