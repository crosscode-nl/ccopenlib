cmake_minimum_required (VERSION 3.9.0)
project (
    TESTS
    LANGUAGES CXX
)

SET(HEADERS
    src/sample1.h
)

SET(SOURCES
    src/sample1.cc
    src/sample1_unittest.cc
)

add_subdirectory(googletest)

add_executable(tests ${HEADERS} ${SOURCES})

target_link_libraries(tests gtest_main ccopenlib)

set(UNIT_TEST tests)

add_test(NAME ${UNIT_TEST} COMMAND ${UNIT_TEST})

target_include_directories(tests PRIVATE src)
