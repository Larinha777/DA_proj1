cmake_minimum_required(VERSION 3.30)
project(DAProject1)

set(CMAKE_CXX_STANDARD 26)

add_executable(DAProject1 main.cpp
        data_structures/Graph.cpp
        data_structures/Graph.h
        data_structures/MutablePriorityQueue.h
        algorithms/Algorithms.cpp
        algorithms/Algorithms.h
        algorithms/util.cpp
        algorithms/util.h)
