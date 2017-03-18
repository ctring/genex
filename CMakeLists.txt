# primary source: https://cmake.org/cmake-tutorial/
cmake_minimum_required(VERSION 3.6)
project(genex)


# inform compiler libs (http://stackoverflow.com/questions/11972603/cmake-clang-and-cv11-on-os-x-10-8)
set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

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



# add genexLib library
include_directories("${PROJECT_SOURCE_DIR}/src")
add_subdirectory(src)



# build the genex command-line interface
add_executable(genexCLI GenexCLI.cpp)
target_link_libraries(genexCLI genexLib)



# testing (http://neyasystems.com/an-engineers-guide-to-unit-testing-cmake-and-boost-unit-tests/)
# this question is similar to ours (http://stackoverflow.com/questions/14446495/cmake-project-structure-with-unit-tests)

enable_testing()

# keep tests in a seperate folder
file(GLOB TEST_SRCS RELATIVE ${PROJECT_SOURCE_DIR} test/*.cpp)

# for each test file
foreach(testSrc ${TEST_SRCS})

  # extract file names (w/o extension)
  get_filename_component(testName ${testSrc} NAME_WE)

  # set compile target
  add_executable(${testName} ${testSrc})


  target_compile_definitions(${testName}  PRIVATE "BOOST_TEST_DYN_LINK=1")

  #link to Boost libraries AND targets and dependencies
  target_link_libraries(${testName} genexLib
                                    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

  # move tests to testOut
  set_target_properties(${testName} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/testOut)

  # add this to test execution
  add_test(NAME ${testName}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_BINARY_DIR}/testOut/${testName})

endforeach(testSrc)
