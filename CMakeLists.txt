cmake_minimum_required (VERSION 3.9.0)
project (
    CCOPENLIB
    VERSION 0.1.0.0
    DESCRIPTION "CCOPENLIB is a free open source library for C++14 that provides commonly needed patterns."
    LANGUAGES CXX
)

include(CTest)
enable_testing()

set(BUILD_QUALITY "Alpha")

SET(HEADERS
        include/ccol/thread/threadpool.hxx
        include/ccol/thread/timer.hxx
        include/ccol/thread/thread_wrapper.hxx
        include/ccol/version/version.hxx
        include/ccol/util/always_false.hxx
)

SET(SOURCES
        src/ccol/thread/threadpool.cxx
        src/ccol/thread/timer.cxx
        src/ccol/thread/thread_wrapper.cxx
)

string(TIMESTAMP BUILD_DATE "%Y-%m-%dT%H:%M:%SZ" UTC)

configure_file(include/ccol/version/version.hxx.in include/ccol/version/version.hxx @ONLY)

add_library(ccopenlib ${HEADERS} ${SOURCES})

target_include_directories(ccopenlib PUBLIC include)

set_property(TARGET ccopenlib PROPERTY PUBLIC_HEADER ${HEADERS})
set_property(TARGET ccopenlib PROPERTY CXX_STANDARD 14)

include(documentation.cmake)

add_subdirectory(tests)

install(TARGETS ccopenlib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/ccopenlib
    ARCHIVE DESTINATION lib/static)
