project(pygenex)

if(APPLE)
    set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif(APPLE)

find_package(PythonLibs 2.7 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIR})
add_library(pygenex SHARED "GENEXBindings.cpp")
target_link_libraries(pygenex genexLib ${PYTHON_LIBRARIES} ${Boost_LIBRARIES})

# don't prepend wrapper library name with lib
set_target_properties(pygenex PROPERTIES PREFIX "")
# move the library to root of build
set_target_properties(pygenex PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
