cmake_minimum_required(VERSION 3.10)

add_executable(client ${CMAKE_CURRENT_SOURCE_DIR}/client.cpp)

include_directories(${CMAKE_SOURCE_DIR}/libs/cpp-httplib)