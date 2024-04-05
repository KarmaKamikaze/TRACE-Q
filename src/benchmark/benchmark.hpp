#ifndef TRACE_Q_BENCHMARK_HPP
#define TRACE_Q_BENCHMARK_HPP

#include <chrono>

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
    long long int funcTime(F func, Args&&... args){
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        func(std::forward<Args>(args)...);
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-t1).count();
    }

}
#endif //TRACE_Q_BENCHMARK_HPP
