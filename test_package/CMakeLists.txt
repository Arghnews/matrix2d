cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

find_package(matrix2d REQUIRED)

# set(CXX_STANDARD 17)
# set(CXX_STANDARD_REQUIRED ON)
# set(CXX_EXTENSIONS OFF)

add_executable(example example.cpp)
set(LIBS matrix2d::matrix2d)
# message(DEBUG "The libs imported: ${LIBS}")
target_link_libraries(example ${LIBS})
set_target_properties(example PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
