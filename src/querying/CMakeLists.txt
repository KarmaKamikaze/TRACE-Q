add_library(querying "")

target_sources(querying
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Range_Query.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Range_Query_Test.cpp
        ${CMAKE_CURRENT_LIST_DIR}/KNN_Query.cpp
        ${CMAKE_CURRENT_LIST_DIR}/KNN_Query_Test.cpp
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Range_Query.hpp
        ${CMAKE_CURRENT_LIST_DIR}/Range_Query_Test.hpp
        ${CMAKE_CURRENT_LIST_DIR}/KNN_Query.hpp
        ${CMAKE_CURRENT_LIST_DIR}/KNN_Query_Test.hpp
)

target_link_libraries(querying "${PQXX_LIBRARIES}")

target_include_directories(querying
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}
)
