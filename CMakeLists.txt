cmake_minimum_required(VERSION 3.3)
project(OS_HW2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp pi_lockfree.c pi_lockfree.h)
add_executable(OS_HW2 ${SOURCE_FILES})