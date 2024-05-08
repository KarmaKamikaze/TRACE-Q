add_library(traceq_benchmarks "")

target_sources(traceq_benchmarks
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/TRACE_Q_Benchmark.cpp
        ${CMAKE_CURRENT_LIST_DIR}/F1.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Benchmark.cpp
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/TRACE_Q_Benchmark.hpp
        ${CMAKE_CURRENT_LIST_DIR}/F1.hpp
        ${CMAKE_CURRENT_LIST_DIR}/Benchmark.hpp
)

target_link_libraries(traceq_benchmarks "${PQXX_LIBRARIES}" logging trajectory_data_handling)

target_include_directories(traceq_benchmarks
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)
