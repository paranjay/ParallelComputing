cmake_minimum_required(VERSION 3.3)
project(SerialAMR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(SerialAMR ${SOURCE_FILES} PThreadVersion.cpp)