add_library(simp-algorithms "")

target_sources(simp-algorithms
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/MRPA.cpp
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/MRPA.hpp
)

target_include_directories(simp-algorithms
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}
)
