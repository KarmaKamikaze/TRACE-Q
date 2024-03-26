add_library(querying "")

target_sources(querying
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/Range_Query.cpp
            ${CMAKE_CURRENT_LIST_DIR}/KNN_Query.cpp
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/Range_Query.hpp
            ${CMAKE_CURRENT_LIST_DIR}/KNN_Query.hpp
)

target_include_directories(querying
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}
)
