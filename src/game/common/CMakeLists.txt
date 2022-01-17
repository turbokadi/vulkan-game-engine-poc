cmake_minimum_required(VERSION 3.17)
project(game_common
        VERSION 0.0.1
        LANGUAGES CXX)

file(GLOB SRC_FILES "*.cpp")
add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} engine)

target_include_directories( ${PROJECT_NAME}
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)


