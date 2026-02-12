cmake_minimum_required(VERSION 3.14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(CodeNebula VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

add_executable(CodeNebula 
    src/main.cpp 
    src/Simulation.cpp
)

target_link_libraries(CodeNebula PRIVATE raylib)