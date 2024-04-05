#include "../src/benchmark/benchmark.hpp"
#include "../src/benchmark/benchmark.cpp"
#include <doctest/doctest.h>

void time_func(int t) {
    _sleep(t);
}


TEST_CASE("test see if benchmark time is above a specified time") {

    SUBCASE("600 ms")
        int t;
        auto t = 600;
        auto duration = analytics::funcTime(time_func, t);
        CHECK(t <= duration);

    SUBCASE("1000 ms")
        int s;
        auto s = 1000;
        auto durs = analytics::funcTime(time_func, s);
    //    std::cout<<"expect 1000: "<<analytics::funcTime(time_func,s)<<"\n";
        CHECK(s <= durs);

}

