

#ifndef TRACE_Q_BENCHMARK_HPP
#define TRACE_Q_BENCHMARK_HPP

#include <iosfwd>
#include <vector>
#include <chrono>
#include <iostream>
#include <source_location>
#include <complex>
#include <utility>

namespace analytics {

    /**
     * This function takes a function and its parameters and times how long times it takes to execute it.
     * This is recorded in milliseconds.
     * @tparam F
     * @tparam Args
     * @param func The function that is measured
     * @param args The measured functions parameters.
     * @return The duration of the input function. Endtime-starttime.
     */
    template<typename F, typename... Args>
    long long int funcTime(F func, Args&&... args);

}
#endif //TRACE_Q_BENCHMARK_HPP
