# primary source: https://cmake.org/cmake-tutorial/
cmake_minimum_required(VERSION 3.6)
project(genex)


# inform compiler libs (http://stackoverflow.com/questions/11972603/cmake-clang-and-cv11-on-os-x-10-8)
set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
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
find_package(Boost 1.6 REQUIRED COMPONENTS unit_test_framework)
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
