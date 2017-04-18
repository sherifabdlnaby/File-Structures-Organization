cmake_minimum_required(VERSION 3.6)
project(FS102)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp
        printer.cpp
        printer.h)

add_executable(FS102 ${SOURCE_FILES})