# Minimum required CMake version
cmake_minimum_required(VERSION 3.10)

# Project name
project(MyProject)

# Add 'include/' folder to the include path
include_directories(include)

# Create a STATIC library from lib.cpp
add_library(lib STATIC src/lib.cpp)

# Build the executable 'myprog'
add_executable(myprog src/main.cpp)

# Link the static library 'lib' to 'myprog'
target_link_libraries(myprog PRIVATE lib)