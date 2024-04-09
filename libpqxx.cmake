# Downloads and compiles libpqxx (Official C++ PostgreSQL) library
include(FetchContent)
set(FETCHCONTENT_QUIET ON)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

set(DSKIP_BUILD_TEST ON CACHE BOOL "Skips compiling libpqxx's tests")
# On Windows we generally recommend building libpqxx as a shared
# library.  On other platforms, we recommend a static library.
IF (WIN32)
    set(BUILD_SHARED_LIBS ON CACHE BOOL "Build a shared library")
ELSE()
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build a static library")
ENDIF()

FetchContent_Declare(libpqxx
        GIT_REPOSITORY https://github.com/jtv/libpqxx.git
        GIT_TAG 7.9.0   # "main" for latest
        GIT_SHALLOW TRUE  # download specific revision only (git clone --depth 1)
        GIT_PROGRESS TRUE # show download progress in Ninja
        USES_TERMINAL_DOWNLOAD TRUE)
set(PQXX_LIBRARIES pqxx)
FetchContent_MakeAvailable(libpqxx)
