# Download and unpack googletest at configure time
configure_file(../cmake/googletest.cmake.in googletest-download/CMakeLists.txt)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/googletest-download )
if (result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/googletest-download )
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
  compressed_pair.cpp
  optional.cpp
  small_vector.cpp)

target_link_libraries(umlaut_test Umlaut::Umlaut gtest_main)

add_test(all_tests umlaut_test)

add_custom_target(check
  COMMAND ${CMAKE_CTEST_COMMAND} --verbose
  DEPENDS umlaut_test)
