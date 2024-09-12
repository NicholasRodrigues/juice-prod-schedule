cmake_minimum_required(VERSION 3.22.1)
project(juice_prod_schedule)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(juice_prod_schedule
        include/algorithm.h
        include/parser.h
        src/algorithm.cpp
        src/main.cpp
        src/parser.cpp)
