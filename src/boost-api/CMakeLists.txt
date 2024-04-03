add_library(boost-api "")

target_sources(boost-api
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/endpoint_handlers.cpp
        ${CMAKE_CURRENT_LIST_DIR}/start-api.cpp
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/endpoint_handlers.hpp
        ${CMAKE_CURRENT_LIST_DIR}/start-api.hpp
)

target_include_directories(boost-api
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)
