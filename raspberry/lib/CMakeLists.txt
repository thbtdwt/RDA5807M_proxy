cmake_minimum_required (VERSION 3.7)
project (RDA5807M_proxy)

include_directories(../include)

add_library (RDA5807M_proxy SHARED RDA5807M_proxy.c)