project(genexCLI)

add_executable(genexCLI GenexCLI.cpp)
target_link_libraries(genexCLI ${Readline_LIBRARY} genexLib)