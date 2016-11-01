cmake_minimum_required(VERSION 3.6)
project(polyphase_external_sort)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp FibonacciGenerator.h Cone.h Tape.h Distributor.h ConeGenerator.h MergingHandler.h)
add_executable(polyphase_external_sort ${SOURCE_FILES})