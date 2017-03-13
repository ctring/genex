# primary source: https://cmake.org/cmake-tutorial/
cmake_minimum_required(VERSION 3.6)
project(genex)

# force c++ (http://stackoverflow.com/questions/37621342/cmake-will-not-compile-to-c-11-standard)
set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# inform compiler libs (http://stackoverflow.com/questions/11972603/cmake-clang-and-cv11-on-os-x-10-8)
set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

# give option to allow exact answers
option(FIND_EXACT "Find exact matches" ON)

# configure header file to include CMAKE values
configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)

# allow src files to see generated config.h
include_directories ("${PROJECT_BINARY_DIR}")

# set the main file
set(SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/Test.cpp
)

# create executable from Test.cpp named genex
add_executable(genex ${SOURCE})
