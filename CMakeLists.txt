cmake_minimum_required(VERSION 3.17)
project(minecraftButActuallyGood)

set(CMAKE_CXX_STANDARD 17)

add_compile_options("-Wall")
add_compile_options("-Wextra")
add_compile_options("-O0")
add_compile_options("-g")

add_executable(${PROJECT_NAME} src/main.cpp)

add_subdirectory(src)
add_subdirectory(doc)
