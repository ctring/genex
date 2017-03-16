project(genexLib)

file(GLOB SRC_FILES RELATIVE ${PROJECT_SOURCE_DIR} *.cpp)

add_library(genexLib ${SRC_FILES})
