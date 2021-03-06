cmake_minimum_required (VERSION 3.9.0)
project (
    CCOPENLIB
    VERSION 1.2.1.0
    DESCRIPTION "CCOPENLIB is a free open source library for C++14 that provides commonly needed patterns."
    LANGUAGES CXX
)

include(CTest)
enable_testing()

#set(BUILD_QUALITY "Alpha")
#set(BUILD_QUALITY "Beta")
set(BUILD_QUALITY "Release")
#set(BUILD_QUALITY "Release candidate")

# Change ABI version when ABI of existing classes has been changed.
set(ABIVERSION 1.2.0.0)

SET(HEADERS
        include/ccol/thread/threadpool.hxx
        include/ccol/thread/timer.hxx
        include/ccol/thread/thread_wrap.hxx
        include/ccol/version/version.hxx
        include/ccol/util/always_false.hxx
        include/ccol/util/cancellationtoken.hxx
        include/ccol/util/cancellationtokensource.hxx
        include/ccol/event/baseevent.hxx
        include/ccol/event/callbackevent.hxx
        include/ccol/event/dataevent.hxx
        include/ccol/event/eventqueue.hxx
        include/ccol/event/callbackeventqueue.hxx
)

SET(SOURCES
        src/ccol/thread/threadpool.cxx
        src/ccol/thread/timer.cxx
        src/ccol/thread/thread_wrap.cxx
        src/ccol/util/cancellationtoken.cxx
        src/ccol/util/cancellationtokensource.cxx
        src/ccol/event/baseevent.cxx
        src/ccol/event/callbackevent.cxx
        src/ccol/event/eventqueue.cxx
        src/ccol/event/callbackeventqueue.cxx
)

string(TIMESTAMP BUILD_DATE "%Y-%m-%dT%H:%M:%SZ" UTC)

configure_file(include/ccol/version/version.hxx.in include/ccol/version/version.hxx @ONLY)

add_library(ccopenlib ${HEADERS} ${SOURCES})

target_include_directories(ccopenlib PUBLIC ${PROJECT_SOURCE_DIR}/include)

set_property(TARGET ccopenlib PROPERTY PUBLIC_HEADER ${HEADERS})

set_target_properties(ccopenlib PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(ccopenlib
      PROPERTIES VERSION ${PROJECT_VERSION}
      SOVERSION ${ABIVERSION}
)

include(documentation.cmake)

add_subdirectory(tests)

install(TARGETS ccopenlib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/ccopenlib
    ARCHIVE DESTINATION lib/static)
