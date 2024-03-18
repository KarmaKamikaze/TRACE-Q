cmake_minimum_required(VERSION 3.28)
project(TRACE_Q CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
        ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

add_definitions(-DSQLITE_ENABLE_RTREE=1)

include(sanitizers.cmake)
include(doctest.cmake)

enable_testing()

# Add subdirectories
add_subdirectory(src)
target_link_libraries(TRACE_Q LINK_PUBLIC sqlite3)
