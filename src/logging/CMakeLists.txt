add_library(logging "")

target_sources(logging
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Logger.cpp
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Logger.hpp
)

target_include_directories(logging
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)
