# primary source: https://cmake.org/cmake-tutorial/
cmake_minimum_required(VERSION 3.6)
project(genex)

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

# collect cpp files
FILE(GLOB SRCFILES *.cpp *.hpp)
add_executable(genex ${SRCFILES})

# testing (http://neyasystems.com/an-engineers-guide-to-unit-testing-cmake-and-boost-unit-tests/)
# enable cmake to run tests
enable_testing()

# keep tests in a seperate folder
file(GLOB TEST_SRCS RELATIVE ${PROJECT_BINARY_DIR} test/*.cpp)

# for each test file
foreach(testSrc ${TEST_SRCS})

  # extract file names (w/o extension)
  get_filename_component(testName ${testSrc} NAME_WE)

  # set compile target
  add_executable(${testName} ${testSrc})

  #link to Boost libraries AND targets and dependencies
  target_link_libraries(${testName}
                                    ${PYTHON_LIBRARIES}
                                    ${Boost_LIBRARIES}
                                    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                                    ${Boost_FILESYSTEM_LIBRARY}
                                    ${Boost_PYTHON_LIBRARY}
                                    ${Boost_IOSTREAMS_LIBRARY})

  # move tests to testOut
  set_target_properties(${testName} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY  ${PROJECT_BINARY_DIR}/testOut)

  # add this to test execution
  add_test(NAME ${testName} 
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/testOut
    COMMAND ${PROJECT_BINARY_DIR}/testOut/${testName})

endforeach(testSrc)

# create executable from Test.cpp named genex
# link libraries 
target_link_libraries(genex
                             ${PYTHON_LIBRARIES}
                             ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                             ${Boost_FILESYSTEM_LIBRARY}
                             ${Boost_PYTHON_LIBRARY}
                             ${Boost_IOSTREAMS_LIBRARY})
