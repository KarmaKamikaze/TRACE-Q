add_executable(TRACE_Q main.cpp)

add_subdirectory(simp-algorithms)

target_link_libraries(TRACE_Q
        PRIVATE
            simp-algorithms
)