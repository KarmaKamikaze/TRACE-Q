add_library(trajectory_data_handling "")

target_sources(trajectory_data_handling
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/sqlite_querying.cpp
            ${CMAKE_CURRENT_LIST_DIR}/trajectory_file_manager.cpp
            ${CMAKE_CURRENT_LIST_DIR}/trajectory_sql.cpp
            ${CMAKE_CURRENT_LIST_DIR}/../../external/sqlite/sqlite3.c
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/sqlite_querying.hpp
            ${CMAKE_CURRENT_LIST_DIR}/trajectory_file_manager.hpp
            ${CMAKE_CURRENT_LIST_DIR}/trajectory_sql.hpp
            ${CMAKE_CURRENT_LIST_DIR}/../../external/sqlite/sqlite3.h
)

target_include_directories(trajectory_data_handling
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/../../external/sqlite
)
