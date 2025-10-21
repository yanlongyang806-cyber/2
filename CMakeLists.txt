cmake_minimum_required(VERSION 3.10)
project(veh_patch LANGUAGES CXX)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}bin)

add_library(veh_patch SHARED veh_patch.cpp)

# 目标平台：64 位
set_target_properties(veh_patch PROPERTIES
    OUTPUT_NAME veh_patch
    PREFIX 
    SUFFIX .dll
)
