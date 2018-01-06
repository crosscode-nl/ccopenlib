option(BUILD_DOC "Build documentation" ON)

if (BUILD_DOC)

    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        set(LOGO_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/ccopenliblogo200x29.png)
        set(LOGO_OUT ${CMAKE_CURRENT_BINARY_DIR}/docs/ccopenliblogo200x29.png)
        set(HEADER_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/header.html)
        set(HEADER_OUT ${CMAKE_CURRENT_BINARY_DIR}/docs/header.html)
        set(FOOTER_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/footer.html)
        set(FOOTER_OUT ${CMAKE_CURRENT_BINARY_DIR}/docs/footer.html)
        set(CUSTOMDOXYGENCSS_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/doxygenccopenlib.css)
        set(CUSTOMDOXYGENCSS_OUT ${CMAKE_CURRENT_BINARY_DIR}/docs/doxygenccopenlib.css)

        configure_file(${LOGO_IN} ${LOGO_OUT} COPYONLY)
        configure_file(${HEADER_IN} ${HEADER_OUT} COPYONLY)
        configure_file(${FOOTER_IN} ${FOOTER_OUT} COPYONLY)
        configure_file(${CUSTOMDOXYGENCSS_IN} ${CUSTOMDOXYGENCSS_OUT} COPYONLY)
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

        add_custom_target( doc_doxygen
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
    else (DOXYGEN_FOUND)
        message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)

endif(BUILD_DOC)
