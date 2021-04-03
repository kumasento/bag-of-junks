cmake_minimum_required(VERSION 3.1)

project(BagOfJunks LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(cache-sim)
add_subdirectory(arith-parser)
add_subdirectory(tetris)
add_subdirectory(memory)
# add_subdirectory(rubik)
add_subdirectory(dots-and-boxes)
add_subdirectory(hex)
add_subdirectory(spreadsheet)
add_subdirectory(keycomb)

# algo
add_subdirectory(algo)
