# Copyright Marcus Larsson 2018
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.5)
project(Umlaut VERSION 0.1.0)

# Config options
option(UMLAUT_ENABLE_TESTS "Enable building the unit tests which depend on catch2" ON)

if (CMAKE_COMPILER_IS_GNUCC)
  option(UMLAUT_ENABLE_COVERAGE "Enable test coverage reporting for gcc/clang" OFF)
endif()

# Compiler warnings
if (MSVC)
  add_compile_options("/W4")
else()
  add_compile_options("-Wpedantic")
  add_compile_options("-Wall")
  add_compile_options("-Wextra")
endif()

# Add Umlaut library
include(CheckCXXCompilerFlag)

add_library(Umlaut INTERFACE)
add_library(Umlaut::Umlaut ALIAS Umlaut)

target_compile_features(Umlaut INTERFACE cxx_std_17)

target_include_directories(Umlaut INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

# Add install target
include(GNUInstallDirs)

set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${version_config}"
  COMPATIBILITY SameMajorVersion)

configure_package_config_file(
  "cmake/Config.cmake.in"
  "${project_config}"
  INSTALL_DESTINATION "${config_install_dir}")

install(TARGETS Umlaut
  EXPORT "${TARGETS_EXPORT_NAME}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(DIRECTORY "include/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  FILES_MATCHING PATTERN "*.hpp")

install(FILES "${project_config}" "${version_config}"
  DESTINATION "${config_install_dir}")

install(EXPORT "${TARGETS_EXPORT_NAME}"
  NAMESPACE "${PROJECT_NAME}::"
  DESTINATION "${config_install_dir}")

# Add tests
if (UMLAUT_ENABLE_TESTS)
  add_subdirectory(test)
endif()

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
