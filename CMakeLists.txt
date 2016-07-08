cmake_minimum_required(VERSION 3.2)

project(trantor)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fpermissive")
set(SRC src)
add_subdirectory(src)
