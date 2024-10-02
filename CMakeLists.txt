cmake_minimum_required(VERSION 3.22.1)
project(juice_prod_schedule)

# add optimization flags
# g++ *.cpp -o apa -O3 -fdata-sections -ffunction-sections
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fdata-sections -ffunction-sections")
set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(juice_prod_schedule
        include/algorithm.h
        include/parser.h
        include/neighborhoods.h
        src/algorithm.cpp
        src/main.cpp
        src/parser.cpp
        src/neighborhoods.cpp)
