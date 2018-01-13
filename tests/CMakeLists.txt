cmake_minimum_required (VERSION 3.9.0)
project (
    TESTS
    LANGUAGES CXX
)

SET(HEADERS
)

SET(SOURCES
    src/ccol/thread/threadpool_unittest.cxx
    src/ccol/thread/timer_unittest.cxx
    src/ccol/thread/thread_wrap_unittest.cxx
    src/ccol/util/cancellationtokensource_unittest.cxx
    src/ccol/event/eventqueue_unittest.cxx
)

add_subdirectory(googletest)

add_executable(tests ${HEADERS} ${SOURCES})

target_link_libraries(tests gtest_main ccopenlib)

set_property(TARGET tests PROPERTY CXX_STANDARD 14)

set(UNIT_TEST tests)

add_test(NAME ${UNIT_TEST} COMMAND ${UNIT_TEST})

target_include_directories(tests PRIVATE src)
