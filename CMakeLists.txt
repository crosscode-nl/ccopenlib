cmake_minimum_required (VERSION 3.9.0)
project (CCOPENLIB)

SET(HEADERS	
	include/threadpool.h
	include/timer.h
) 

SET(SOURCES	
	src/threadpool.cpp
	src/timer.cpp
)

add_library(ccopenlib ${HEADERS} ${SOURCES})

target_include_directories(ccopenlib PUBLIC include)

set_property(TARGET ccopenlib PROPERTY PUBLIC_HEADER ${HEADERS})

install(TARGETS ccopenlib 	
    LIBRARY DESTINATION lib
	PUBLIC_HEADER DESTINATION include/ccopenlib
    ARCHIVE DESTINATION lib/static)
