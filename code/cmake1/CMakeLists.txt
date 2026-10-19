# Minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Project name
project(MyProject)

# Combine both main.cpp and lib.cpp
add_executable(myprog main.cpp lib.cpp)
