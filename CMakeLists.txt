cmake_minimum_required(VERSION 3.5)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
project(boostCompute)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")




set(SOURCE_FILES main.cpp iterators.h)
add_executable(boostCompute ${SOURCE_FILES})

find_package( OpenCL REQUIRED )
include_directories($ENV{HOME}/Documents/Development/boost_1_61_0 ${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})
target_link_libraries (boostCompute ${OpenCL_LIBRARY})
MESSAGE( STATUS "OpenCL_LIBRARY : " ${OpenCL_LIBRARY} " OpenCL_INCLUDE_DIRS : "  ${OpenCL_INCLUDE_DIRS})