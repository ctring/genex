# primary source: https://cmake.org/cmake-tutorial/
cmake_minimum_required(VERSION 2.8)
project(genex)

option(BUILD_CLI "build the GENEX command-line interface" ON)
option(BUILD_TESTS "build tests" ON)
option(BUILD_PYGENEX "build the Python version of GENEX" ON)

message("Building GENEX command-line interface: ${BUILD_CLI}")
message("Building tests: ${BUILD_TESTS}")
message("Building PYGENEX: ${BUILD_PYGENEX}")

if (UNIX AND NOT APPLE)
  # The next 2 lines might need to be removed if the following error occurs:
  #   "sorry - this program has been built without plugin support"
  set(CMAKE_CXX_FLAGS "-std=c++11 -O2 -flto -fPIC -pthread")
else()
  set(CMAKE_CXX_FLAGS "-std=c++11 -O2 -flto -stdlib=libc++")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# include GNU readline
find_package(Readline REQUIRED)
include_directories(${Readline_INCLUDE_DIRS})

# include Boost
set(BOOST_COMPONENTS serialization iostreams)
if (BUILD_PYGENEX)
  set(BOOST_COMPONENTS ${BOOST_COMPONENTS} python)
endif()

if (BUILD_TESTS)
  set(BOOST_COMPONENTS ${BOOST_COMPONENTS} unit_test_framework)
endif()
find_package(Boost 1.65 REQUIRED COMPONENTS ${BOOST_COMPONENTS})

add_library(boostSerialization UNKNOWN IMPORTED)
set_property(TARGET boostSerialization PROPERTY IMPORTED_LOCATION "${Boost_SERIALIZATION_LIBRARY}")
add_library(boostIOstreams UNKNOWN IMPORTED)
set_property(TARGET boostIOstreams PROPERTY IMPORTED_LOCATION "${Boost_IOSTREAMS_LIBRARY}")
add_library(boostTest UNKNOWN IMPORTED)
set_property(TARGET boostTest PROPERTY IMPORTED_LOCATION "${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}")
add_library(boostPython UNKNOWN IMPORTED)
set_property(TARGET boostPython PROPERTY IMPORTED_LOCATION "${Boost_PYTHON_LIBRARY}")

include_directories(${Boost_INCLUDE_DIRS})

# add genexLib library
include_directories("${PROJECT_SOURCE_DIR}/genex")
add_subdirectory(genex)

# build the genex command-line interface
if (BUILD_CLI)
  add_subdirectory(cli)
endif()

# build pygenex
if (BUILD_PYGENEX)
  add_subdirectory(pygenex)
endif()

# testing (http://neyasystems.com/an-engineers-guide-to-unit-testing-cmake-and-boost-unit-tests/)
if (BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

