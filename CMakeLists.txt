cmake_minimum_required(VERSION 3.3)
project(ParallelComputing)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

add_executable(ParallelComputing ${SOURCE_FILES} PThreadVersion.cc PersistentThread.cc OmpVersion.cpp)