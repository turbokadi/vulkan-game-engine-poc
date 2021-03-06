cmake_minimum_required(VERSION 3.17)
project(engine
        VERSION 0.0.1
        LANGUAGES CXX)

file(GLOB SRC_FILES "*.cpp")

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

target_link_libraries(${PROJECT_NAME}
        glfw
        vulkan)

target_link_libraries(${PROJECT_NAME} game_common)

target_include_directories( ${PROJECT_NAME}
                            PUBLIC
                            $<INSTALL_INTERFACE:include>
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> )

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
add_custom_command( OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/__spv # Always compile because target never exist
                    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/compile_shader.sh CompileShader
                    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/spv/* ${CMAKE_BINARY_DIR}/shaders
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    COMMENT "Compiling Shaders")

add_custom_target(CompileShader DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/__spv)
