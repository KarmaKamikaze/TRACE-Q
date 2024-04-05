#include "benchmark.hpp"

namespace analytics {

    template<typename F, typename... Args>
    long long int funcTime(F func, Args&&... args){
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        func(std::forward<Args>(args)...);
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-t1).count();
    }
}
