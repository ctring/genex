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


# include boost
find_package(Boost 1.66 REQUIRED 
             COMPONENTS unit_test_framework python3 serialization iostreams)
include_directories(${Boost_INCLUDE_DIRS})

# include GNU readline
find_package(Readline REQUIRED)
include_directories(${Readline_INCLUDE_DIRS})

# add genexLib library
include_directories("${PROJECT_SOURCE_DIR}/genex")
add_subdirectory(genex)

# build the genex command-line interface
add_subdirectory(cli)

# build pygenex
add_subdirectory(pygenex)

# testing (http://neyasystems.com/an-engineers-guide-to-unit-testing-cmake-and-boost-unit-tests/)
enable_testing()
add_subdirectory(test)
