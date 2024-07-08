cmake_minimum_required(VERSION 3.10)

add_executable(server ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp)

include_directories(${CMAKE_SOURCE_DIR}/libs/cpp-httplib)