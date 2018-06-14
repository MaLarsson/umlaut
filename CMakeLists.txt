# Main CMakeLists.txt
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

# Add google test library
include(cmake/DownloadProject.cmake)

download_project(
  PROJ googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG master
  UPDATE_DISCONNECTED 1)

add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

# Add tests
include(CTest)
enable_testing()

add_executable(umlaut_test test/test.cpp)
target_link_libraries(umlaut_test umlaut gtest)

add_test(all_tests umlaut_test)

add_custom_target(tests
  COMMAND ${CMAKE_CTEST_COMMAND} --verbose
  DEPENDS umlaut_test)
