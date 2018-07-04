# Copyright Marcus Larsson 2018
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.5)
project(umlaut)

# Compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")

# Compiler warnings
if (MSVC)
  add_compile_options("/W4")
else()
  add_compile_options("-Wpedantic")
  add_compile_options("-Wall")
  add_compile_options("-Wextra")
endif()

# Add umlaut library
add_library(umlaut INTERFACE)
target_include_directories(umlaut INTERFACE include)

# Download and unpack googletest at configure time
configure_file(cmake/googletest.cmake.in googletest-download/CMakeLists.txt)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if (result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if (result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest, this defines the gtest and gtest_main targets.
add_subdirectory(
  ${CMAKE_BINARY_DIR}/googletest-src
  ${CMAKE_BINARY_DIR}/googletest-build
  EXCLUDE_FROM_ALL)

# Add check target
include(CTest)
enable_testing()

add_executable(umlaut_test EXCLUDE_FROM_ALL
  test/compressed_pair.cpp
  test/small_vector.cpp)

target_link_libraries(umlaut_test umlaut gtest_main)

add_test(all_tests umlaut_test)

add_custom_target(check
  COMMAND ${CMAKE_CTEST_COMMAND} --verbose
  DEPENDS umlaut_test)

# Add doc target
find_package(Doxygen)

if (NOT DOXYGEN_FOUND)
  message(WARNING "Doxygen was not found; the 'doc' and 'docset' targets "
    "will be unavailable.")
  return()
endif()

set(UMLAUT_HTML_OUTPUT       html)
set(UMLAUT_GENERATE_DOCSET   NO)
set(UMLAUT_GENERATE_TREEVIEW NO)
set(UMLAUT_SEARCHENGINE      YES)

configure_file(doc/Doxyfile.in documentation.doxygen @ONLY)
add_custom_target(doc
  COMMAND ${CMAKE_COMMAND} -E remove_directory html
  COMMAND ${DOXYGEN_EXECUTABLE} documentation.doxygen
  COMMENT "Generating API documentation with Doxygen"
  VERBATIM)
