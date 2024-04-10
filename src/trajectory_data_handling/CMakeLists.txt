add_library(trajectory_data_handling "")

target_sources(trajectory_data_handling
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/Query_Handler.cpp
            ${CMAKE_CURRENT_LIST_DIR}/File_Manager.cpp
            ${CMAKE_CURRENT_LIST_DIR}/Trajectory_Manager.cpp
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/Query_Handler.hpp
            ${CMAKE_CURRENT_LIST_DIR}/File_Manager.hpp
            ${CMAKE_CURRENT_LIST_DIR}/Trajectory_Manager.hpp
)

target_link_libraries(trajectory_data_handling "${PQXX_LIBRARIES}")

find_package (SQLite3 REQUIRED)
if (SQLITE3_FOUND)
    target_include_directories(trajectory_data_handling PRIVATE ${SQLite3_INCLUDE_DIRS})
    target_link_libraries (trajectory_data_handling ${SQLITE3_LIBRARIES})
endif (SQLITE3_FOUND)

target_include_directories(trajectory_data_handling
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}
)
