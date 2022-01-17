cmake_minimum_required(VERSION 3.17)

find_package(Doxygen)

set(DOXYGEN_INDEX_FILE ${CMAKE_CURRENT_SOURCE_DIR}/html/index.html)

add_custom_command( OUTPUT ${DOXYGEN_INDEX_FILE}
                    COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    COMMENT "Generating docs"
                    VERBATIM)

add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})