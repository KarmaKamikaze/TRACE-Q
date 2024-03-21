add_executable(TRACE_Q main.cpp
)

add_subdirectory(simp-algorithms)
add_subdirectory(trajectory_data_handling)
add_subdirectory(querying)

target_link_libraries(TRACE_Q
        PRIVATE
            simp-algorithms
            trajectory_data_handling
            querying
)
