cmake_minimum_required (VERSION 3.9.0)
project (
    CCOPENLIB
    VERSION 0.1.0.0
    DESCRIPTION "CCOPENLIB is a lightweigt open source library based on c++14 providing common used patterns."
    LANGUAGES CXX
)


SET(HEADERS	
	include/ccol/thread/threadpool.h
	include/ccol/thread/timer.h
) 

SET(SOURCES	
	src/ccol/thread/threadpool.cpp
	src/ccol/thread/timer.cpp
)

add_library(ccopenlib ${HEADERS} ${SOURCES})

target_include_directories(ccopenlib PUBLIC include)

set_property(TARGET ccopenlib PROPERTY PUBLIC_HEADER ${HEADERS})
set_property(TARGET ccopenlib PROPERTY CXX_STANDARD 14)

install(TARGETS ccopenlib 	
    LIBRARY DESTINATION lib
	PUBLIC_HEADER DESTINATION include/ccopenlib
    ARCHIVE DESTINATION lib/static)
