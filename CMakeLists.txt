cmake_minimum_required (VERSION 3.9.0)
project (
    CCOPENLIB
    VERSION 0.1.0.0
    DESCRIPTION "CCOPENLIB is a free lightweight open source library that is based on C++14 that provides commonly needed patterns. "
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

option(BUILD_DOC "Build documentation" ON)

if (BUILD_DOC)

    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

        add_custom_target( doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
    else (DOXYGEN_FOUND)
        message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)

endif(BUILD_DOC)

install(TARGETS ccopenlib
    LIBRARY DESTINATION lib
	PUBLIC_HEADER DESTINATION include/ccopenlib
    ARCHIVE DESTINATION lib/static)
