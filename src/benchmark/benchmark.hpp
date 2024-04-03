

#ifndef TRACE_Q_BENCHMARK_HPP
#define TRACE_Q_BENCHMARK_HPP

#include <iosfwd>
#include <vector>
#include <chrono>
#include <iostream>
#include <source_location>

class benchmark{

public:
    benchmark()
    {
         m_startpoint = std::chrono::high_resolution_clock::now();
    }

    ~benchmark()
    {
        stop();
    }

    /**  Records time off a functions start and end.
     *   Then calculates the time spend on running the function
     */

    void stop()
    {
        auto endpoint = std::chrono::high_resolution_clock::now();

        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startpoint).time_since_epoch().count();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endpoint).time_since_epoch().count();

        auto duration = end - start;
        double ms = duration * 0.001;

        std::cout << duration << "  us (" << ms << " ms) \n";

    }
private:
    std::chrono::time_point<std::chrono::high_resolution_clock > m_startpoint;

};

#endif //TRACE_Q_BENCHMARK_HPP
