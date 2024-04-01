cmake_minimum_required(VERSION 3.27)

set(PROJECT_NAME AsyncSocketServer)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_subdirectory(src)
