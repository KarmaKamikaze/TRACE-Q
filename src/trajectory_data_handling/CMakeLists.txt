add_library(trajectory_data_handling "")

target_sources(trajectory_data_handling
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/sqlite_querying.cpp
            ${CMAKE_CURRENT_LIST_DIR}/trajectory_file_manager.cpp
            ${CMAKE_CURRENT_LIST_DIR}/trajectory_sql.cpp
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/sqlite_querying.hpp
            ${CMAKE_CURRENT_LIST_DIR}/trajectory_file_manager.hpp
            ${CMAKE_CURRENT_LIST_DIR}/trajectory_sql.hpp
)

find_package (SQLite3 REQUIRED)
if (SQLITE3_FOUND)
    target_include_directories(trajectory_data_handling PRIVATE ${SQLite3_INCLUDE_DIRS})
    target_link_libraries (trajectory_data_handling ${SQLITE3_LIBRARIES})
endif (SQLITE3_FOUND)

target_include_directories(trajectory_data_handling
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}
)
