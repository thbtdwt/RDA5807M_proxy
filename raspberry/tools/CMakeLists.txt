cmake_minimum_required (VERSION 3.7)
project (RDA5807M_tools)

include_directories(../include)

find_library(RDA5807M_PROXY NAMES RDA5807M_proxy PATHS ../lib NO_DEFAULT_PATH)
MESSAGE("RDA5807M_PROXY ${RDA5807M_PROXY}")

add_executable (RDA5807M_ALL_REG RDA5807M_ALL_REG.c)
target_link_libraries (RDA5807M_ALL_REG LINK_PUBLIC ${RDA5807M_PROXY})
