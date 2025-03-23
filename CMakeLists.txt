cmake_minimum_required(VERSION 3.30)
project(kmeans)

set(CMAKE_CXX_STANDARD 20)

add_executable(kmeans main.cpp
        src/Kmeans.cpp
        src/Kmeans.h
        src/Point.cpp
        src/Point.h
        src/Utils.cpp
        src/Utils.h
        src/ThreadPool.cpp
        src/ThreadPool.h
)

