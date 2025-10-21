cmake_minimum_required(VERSION 3.15)
project(veh_patch_project)

set(CMAKE_CXX_STANDARD 17)

# 基础版 DLL
add_library(veh_patch_basic SHARED src/veh_patch_basic.cpp)

# 智能版 DLL
add_library(veh_patch_advanced SHARED src/veh_patch_advanced.cpp)
