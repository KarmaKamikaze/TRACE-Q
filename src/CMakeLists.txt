add_executable(TRACE_Q main.cpp ../external/sqlite/sqlite3.c
        trajectory_data_handling/trajectory_file_manager.cpp
        trajectory_data_handling/sqlite_querying.hpp
        trajectory_data_handling/trajectory_sql.hpp
        data/trajectory_structure.hpp
        trajectory_data_handling/trajectory_file_manager.hpp
        trajectory_data_handling/trajectory_sql.cpp
        trajectory_data_handling/sqlite_querying.cpp)

add_subdirectory(simp-algorithms)

target_link_libraries(TRACE_Q
        PRIVATE
            simp-algorithms
)