# primary source: https://cmake.org/cmake-tutorial/
cmake_minimum_required(VERSION 3.0)
project(genex)

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

# include boost
if (APPLE)
  set(BOOST_PYTHON_NAME "python3")
else()
  set(BOOST_PYTHON_NAME "python")
endif()
find_package(Boost 1.66 REQUIRED
             COMPONENTS unit_test_framework
                        ${BOOST_PYTHON_NAME}
                        serialization
                        iostreams)

include_directories(${Boost_INCLUDE_DIRS})

# add genexLib library
include_directories("${PROJECT_SOURCE_DIR}/genex")
add_subdirectory(genex)

# build the genex command-line interface
add_subdirectory(cli)

# build pygenex
if("${Boost_PYTHON_FOUND} " STREQUAL "ON " OR "${Boost_PYTHON3_FOUND} " STREQUAL "ON ")
  add_subdirectory(pygenex)
else()
  message("-- Boost Python not found. Pygenex will not be built.")
endif()

# testing (http://neyasystems.com/an-engineers-guide-to-unit-testing-cmake-and-boost-unit-tests/)
enable_testing()
add_subdirectory(test)

