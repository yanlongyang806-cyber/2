cmake_minimum_required(VERSION 3.10)
project(veh_patch LANGUAGES CXX)

add_library(veh_patch SHARED veh_patch.cpp)

set_target_properties(veh_patch PROPERTIES
    OUTPUT_NAME "veh_patch"
)
