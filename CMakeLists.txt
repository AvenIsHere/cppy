cmake_minimum_required(VERSION 4.3)
project(cppy)

set(CMAKE_CXX_STANDARD 20)

add_library(cppy SHARED cppy.cpp)
add_executable(cppy_test Test.cpp)
