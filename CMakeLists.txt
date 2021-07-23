cmake_minimum_required(VERSION 3.14)

project(matrix2d)

# set(CMAKE_VERBOSE_MAKEFILE ON)

# Enable building with conan cmake_find_package generator
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(${PROJECT_NAME}_DEVELOPER_FLAGS FALSE CACHE BOOL "Enable warning/sanitizer flags during development of library")
if (${PROJECT_NAME}_DEVELOPER_FLAGS)
    string(JOIN " " COMPILER_FLAGS "-Wall -Wextra -pedantic -Wfloat-equal"
    "-Wwrite-strings -Wswitch-enum -Wunreachable-code -Wconversion"
    "-Wcast-qual -Wstrict-overflow=5 -Werror=uninitialized"
    "-Werror=return-type -pipe -fdiagnostics-color")

    string(APPEND CMAKE_CXX_FLAGS_DEBUG " -g -O0 ${COMPILER_FLAGS} ")

    set(SANITIZERS " -fsanitize=address,undefined ")
    string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fno-omit-frame-pointer ${SANITIZERS} ")
    string(APPEND CMAKE_EXE_LINKER_FLAGS_DEBUG " -fno-omit-frame-pointer ${SANITIZERS} -fuse-ld=lld ")
    string(APPEND CMAKE_CXX_FLAGS_RELEASE " -fno-omit-frame-pointer ${SANITIZERS} ")
    string(APPEND CMAKE_EXE_LINKER_FLAGS_RELEASE " -fno-omit-frame-pointer ${SANITIZERS} -fuse-ld=lld ")

endif()

find_package(fmt REQUIRED)
find_package(range-v3 REQUIRED)
find_package(span-lite REQUIRED)
find_package(Boost QUIET REQUIRED)

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(INSTALL_INCLUDE_DIR "include")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(HEADERS "matrix2d.hpp" "matrix2d.tpp" "index.hpp")
set(SOURCES "")

list(TRANSFORM HEADERS PREPEND "${INCLUDE_DIR}/${PROJECT_NAME}/")
list(TRANSFORM SOURCES PREPEND "${SRC_DIR}/")

add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE
    # Not sure how/if possible to automate this, with generator expressions
    $<BUILD_INTERFACE:${INCLUDE_DIR}/${PROJECT_NAME}/matrix2d.hpp>
    $<BUILD_INTERFACE:${INCLUDE_DIR}/${PROJECT_NAME}/matrix2d.tpp>
    $<BUILD_INTERFACE:${INCLUDE_DIR}/${PROJECT_NAME}/index.hpp>
    $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}/${PROJECT_NAME}/matrix2d.hpp>
    $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}/${PROJECT_NAME}/matrix2d.tpp>
    $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}/${PROJECT_NAME}/index.hpp>
    )
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# INSTALL_INTERFACE should not use absolute paths
target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${INCLUDE_DIR}>
    $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}>
    )

target_link_libraries(${PROJECT_NAME}
    INTERFACE
    nonstd::span-lite
    fmt::fmt
    range-v3::range-v3
    # There is no component for boost::iterator, so just bring in everything I suppose...
    Boost::Boost
    )

enable_testing()
add_subdirectory(tests)

add_custom_target(${PROJECT_NAME}_after_all_built ALL VERBATIM)
add_custom_command(TARGET ${PROJECT_NAME}_after_all_built POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
    copy "${CMAKE_BINARY_DIR}/compile_commands.json" "${CMAKE_SOURCE_DIR}"
    VERBATIM
    )

# Installs the headers and libs
install(TARGETS "${PROJECT_NAME}" EXPORT "${PROJECT_NAME}-targets")

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS "${PROJECT_NAME}" PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/${PROJECT_NAME}")

include(GNUInstallDirs)

# Generate a targets file and install it
install(EXPORT "${PROJECT_NAME}-targets"
        FILE "${PROJECT_NAME}-targets.cmake"
        NAMESPACE "${PROJECT_NAME}::"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
         )

# https://cmake.org/cmake/help/latest/module/CMakePackageConfigHelpers.html
include(CMakePackageConfigHelpers)

####################################

# Generates a package version file in the build directory
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    VERSION 0.2.5
    COMPATIBILITY AnyNewerVersion
    )

# Install the package version file to the install directory
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

####################################

# Generate a cmake package config file
configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

# Install the package config file
install(FILES
          "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
          DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

if (${PROJECT_NAME}_DEVELOPER_FLAGS)
    add_executable(main EXCLUDE_FROM_ALL main.cpp)
    target_link_libraries(main ${PROJECT_NAME}::${PROJECT_NAME} fmt::fmt)
    set_target_properties(main PROPERTIES
        CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
endif()

