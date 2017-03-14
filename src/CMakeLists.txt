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
  "${PROJECT_SOURCE_DIR}/config.hpp.in"
  "${PROJECT_BINARY_DIR}/config.hpp"
)

# allow src files to see generated config.h
include_directories ("${PROJECT_BINARY_DIR}")

# include boost
find_package(Boost 1.61 REQUIRED COMPONENTS python filesystem iostreams unit_test_framework)
set(BOOST_INCLUDEDIR /usr/include)
set(BOOST_LIBRARYDIR /usr/lib)
include_directories(${Boost_INCLUDE_DIRS})
find_package( PythonLibs 2.7 REQUIRED )
find_package( PythonInterp 2.7 REQUIRED )

# set the main file (this could be trouble later on)
set(SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/Test.cpp
)

# create executable from Test.cpp named genex
add_executable(genex ${SOURCE})

# link libraries 
# target_link_libraries(foo ${Boost_LIBRARIES})
target_link_libraries(genex  ${PYTHON_LIBRARIES}
                             ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                             ${Boost_FILESYSTEM_LIBRARY}
                             ${Boost_PYTHON_LIBRARY}
                             ${Boost_IOSTREAMS_LIBRARY})
