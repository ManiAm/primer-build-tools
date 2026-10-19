# Minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Project name
project(MyProject)

# Add 'include/' folder to the include path
include_directories(include)

# Combine both main.cpp and lib.cpp
add_executable(myprog src/main.cpp src/lib.cpp)
