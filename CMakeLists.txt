# primary source: https://cmake.org/cmake-tutorial/
cmake_minimum_required(VERSION 3.0)
project(genex)

if (UNIX AND NOT APPLE)
  set(CMAKE_AR "gcc-ar")
  set(CMAKE_RANLIB "gcc-ranlib")
  set(CMAKE_CXX_FLAGS "-std=c++11 -O2 -flto -pthread")  
else()
  set(CMAKE_CXX_FLAGS "-std=c++11 -O2 -flto -stdlib=libc++")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# configure header file to include CMAKE values
option(SINGLE_PRECISION "Represent data using single-precision floating-point" OFF)
configure_file (
  "${PROJECT_SOURCE_DIR}/config.hpp.in"
  "${PROJECT_BINARY_DIR}/config.hpp"
)

# allow src files to see generated config.hpp
include_directories("${PROJECT_BINARY_DIR}")


# include boost
find_package(Boost 1.60 REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})

# include GNU readline
find_package(Readline REQUIRED)
include_directories(${Readline_INCLUDE_DIRS})

# add genexLib library
include_directories("${PROJECT_SOURCE_DIR}/src")
add_subdirectory(src)

# build the genex command-line interface
add_subdirectory(cli)

# testing (http://neyasystems.com/an-engineers-guide-to-unit-testing-cmake-and-boost-unit-tests/)
enable_testing()
add_subdirectory(test)
