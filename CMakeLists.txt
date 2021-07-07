cmake_minimum_required(VERSION 3.10)

project(MinimalPP VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(minimalpp INTERFACE)
target_include_directories(minimalpp INTERFACE "${PROJECT_SOURCE_DIR}/include")
