cmake_minimum_required(VERSION 3.6)
project(FS101)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Device.cpp Device.h)
add_executable(FS101 ${SOURCE_FILES})