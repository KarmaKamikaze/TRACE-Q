add_library(benchmark "")

target_sources(benchmark
        PRIVATE

        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/benchmark.hpp
)

target_include_directories(benchmark
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)