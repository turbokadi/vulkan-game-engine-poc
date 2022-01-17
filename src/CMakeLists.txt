cmake_minimum_required(VERSION 3.17)

add_subdirectory(game)
add_subdirectory(engine)

target_link_libraries(${PROJECT_NAME} game_common)
