cmake_minimum_required(VERSION 3.10)

project(PairingHeap LANGUAGES CXX)

include(cmake/CompilerWarnings.cmake)
include(cmake/StandardProjectSettings.cmake)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/)
