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

#add_custom_command(
 #    TARGET ${UNIT_TEST}
#     COMMENT "Run tests"
#     POST_BUILD
#     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#     COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -R "^${UNIT_TEST}$" --output-on-failures --debug
#)

target_include_directories(tests PRIVATE src)
