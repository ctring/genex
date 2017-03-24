project(genexCLI)

file(GLOB_RECURSE SRC_FILES RELATIVE ${PROJECT_SOURCE_DIR} *.cpp)

add_executable(genex ${SRC_FILES})

target_link_libraries(genex ${Readline_LIBRARY} genexLib)

set_target_properties(genex PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})