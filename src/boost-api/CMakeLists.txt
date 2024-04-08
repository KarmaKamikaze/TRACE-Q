add_library(boost-api "")

find_package(Boost REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(boost-api PRIVATE ${Boost_LIBRARIES} Ws2_32.lib trajectory_data_handling)

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

