project(genexCLI)

file(GLOB_RECURSE SRC_FILES RELATIVE ${PROJECT_SOURCE_DIR} *.cpp)

add_executable(genexcli ${SRC_FILES})

target_link_libraries(genexcli ${Readline_LIBRARY} genexLib
                               ${Boost_SERIALIZATION_LIBRARY}
                               ${Boost_IOSTREAMS_LIBRARY})

set_target_properties(genexcli PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})