add_library(querying "")

target_sources(querying
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Query.cpp
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Query.hpp
)

target_include_directories(querying
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)
