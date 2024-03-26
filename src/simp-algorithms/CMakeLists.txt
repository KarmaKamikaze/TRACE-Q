add_library(simp-algorithms "")

target_sources(simp-algorithms
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/MRPA.cpp
            ${CMAKE_CURRENT_LIST_DIR}/TRACE_Q.cpp
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/MRPA.hpp
            ${CMAKE_CURRENT_LIST_DIR}/TRACE_Q.hpp
)

target_include_directories(simp-algorithms
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}
)
