project(genexLib)

file(GLOB_RECURSE SRC_FILES RELATIVE ${PROJECT_SOURCE_DIR} *.cpp)

add_library(genexLib ${SRC_FILES})
target_link_libraries(genexLib ${ZLIB_LIBRARIES} ${Boost_LIBRARIES})
