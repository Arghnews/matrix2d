cmake_minimum_required(VERSION 3.14)

project(matrix2d_tests)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

find_package(Catch2 REQUIRED)
find_package(range-v3 REQUIRED)
find_package(fmt REQUIRED)
find_package(span-lite REQUIRED)

set(PARENT_PROJECT "matrix2d")
# If built as top level project
if (${${PARENT_PROJECT}_DEVELOPER_FLAGS})
    string(JOIN " " COMPILER_FLAGS "-Wall -Wextra -pedantic -Wfloat-equal"
    "-Wwrite-strings -Wswitch-enum -Wunreachable-code -Wconversion"
    "-Wcast-qual -Wstrict-overflow=5 -Werror=uninitialized"
    "-Werror=return-type -pipe -fdiagnostics-color")

    string(APPEND CMAKE_CXX_FLAGS_DEBUG " -g -O0 ${COMPILER_FLAGS} ")

    string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fno-omit-frame-pointer -fsanitize=address,undefined ")
    string(APPEND CMAKE_EXE_LINKER_FLAGS_DEBUG " -fno-omit-frame-pointer -fsanitize=address,undefined ")
endif()

add_executable(${PROJECT_NAME} test_cases.cpp main.cpp)
target_link_libraries(${PROJECT_NAME} matrix2d::matrix2d
    Catch2::Catch2 range-v3::range-v3 fmt::fmt
    )

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)

include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME})

