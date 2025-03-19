cmake_minimum_required(VERSION 3.30)
project(DAProject1)

set(CMAKE_CXX_STANDARD 26)

add_executable(DAProject1 main.cpp
        Graph.cpp
        Graph.h
        MutablePriorityQueue.h
        Algorithms.cpp
        Dijkstra.h
        Dijkstra.cpp
        Algorithms.h)
