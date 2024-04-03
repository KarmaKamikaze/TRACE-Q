add_executable(TRACE_Q main.cpp)

add_subdirectory(simp-algorithms)
add_subdirectory(trajectory_data_handling)
add_subdirectory(querying)
add_subdirectory(boost-api)

find_package(Boost REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIRS})

target_link_libraries(TRACE_Q
        PRIVATE
            simp-algorithms
            trajectory_data_handling
            querying
            boost-api
            sqlite3
            Ws2_32.lib
            ${Boost_LIBRARIES}
)
