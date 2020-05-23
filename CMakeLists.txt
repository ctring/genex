# primary source: https://cmake.org/cmake-tutorial/
cmake_minimum_required(VERSION 3.0)
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
  set(CMAKE_AR "gcc-ar")
  set(CMAKE_RANLIB "gcc-ranlib")
  set(CMAKE_CXX_FLAGS "-std=c++11 -O2 -flto -fPIC -pthread")
else()
  set(CMAKE_CXX_FLAGS "-std=c++11 -O2 -flto -stdlib=libc++")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# include GNU readline
find_package(Readline REQUIRED)
include_directories(${Readline_INCLUDE_DIRS})

# See "Boost CMake" section in https://cmake.org/cmake/help/latest/module/FindBoost.html
# This new feature breaks FindBoost of this project so disable it
set(Boost_NO_BOOST_CMAKE ON)

# include Boost
# From FindBoost documentation:
# "Note that Boost Python components require a Python version suffix
# (Boost 1.67 and later), e.g. ``python36`` or ``python27`` for the
# versions built against Python 3.6 and 2.7, respectively.  This also
# applies to additional components using Python including
# ``mpi_python`` and ``numpy``.  Earlier Boost releases may use
# distribution-specific suffixes such as ``2``, ``3`` or ``2.7``.
# These may also be used as suffixes, but note that they are not
# portable.""
set(BOOST_COMPONENTS serialization iostreams)
if (BUILD_PYGENEX)
  set(BOOST_COMPONENTS ${BOOST_COMPONENTS} python27)
endif()
if (BUILD_TESTS)
  set(BOOST_COMPONENTS ${BOOST_COMPONENTS} unit_test_framework)
endif()
find_package(Boost 1.72 REQUIRED COMPONENTS ${BOOST_COMPONENTS})

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

