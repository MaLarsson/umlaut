# Clone Catch2 repository at configure time
find_package(Git)

if(NOT GIT_FOUND)
  message(FATAL_ERROR "git not found!")
endif()

set(CATCH2_URL "https://github.com/catchorg/Catch2.git")
set(LIB_DIR "${CMAKE_BINARY_DIR}/lib")
set(CATCH2_DIR "${LIB_DIR}/Catch2")

file(MAKE_DIRECTORY ${LIB_DIR})

if(EXISTS ${CATCH2_DIR})
  execute_process(
    COMMAND ${GIT_EXECUTABLE} pull origin master
    WORKING_DIRECTORY ${LIB_DIR}
    OUTPUT_VARIABLE git_output)
else()
  execute_process(
    COMMAND ${GIT_EXECUTABLE} clone ${CATCH2_URL}
    WORKING_DIRECTORY ${LIB_DIR}
    OUTPUT_VARIABLE git_output)
endif()

message("${git_output}")

# Add Catch2, this defines the Catch2::Catch2 target.
add_subdirectory(${CATCH2_DIR} ${CATCH2_DIR} EXCLUDE_FROM_ALL)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CATCH2_DIR}/contrib")

# Add check target
add_executable(umlaut_test EXCLUDE_FROM_ALL
  main.cpp
  compressed_pair.cpp
  optional.cpp
  small_vector.cpp)

target_link_libraries(umlaut_test Umlaut::Umlaut Catch2::Catch2)

if (CMAKE_COMPILER_IS_GNUCC AND UMLAUT_ENABLE_COVERAGE)
  target_compile_options(umlaut_test PRIVATE --coverage)
  target_link_libraries(umlaut_test PRIVATE --coverage)
endif()

include(CTest)
include(Catch)

catch_discover_tests(umlaut_test)

add_custom_target(check
  COMMAND ${CMAKE_CTEST_COMMAND} --verbose
  DEPENDS umlaut_test)
