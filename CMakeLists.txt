cmake_minimum_required(VERSION 3.28)
project(TRACE_Q)

set(CMAKE_CXX_STANDARD 20)

add_executable(TRACE_Q main.cpp
        Simp-Algorithms/MRPA.cpp
        Simp-Algorithms/MRPA.h)
