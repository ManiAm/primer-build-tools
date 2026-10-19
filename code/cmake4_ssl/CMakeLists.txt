cmake_minimum_required(VERSION 3.10)

project(ExampleApp CXX)

# Use find_package to locate OpenSSL
find_package(OpenSSL REQUIRED)

# Add the executable
add_executable(myprog main.cpp)

# Link against OpenSSL libraries
target_link_libraries(myprog OpenSSL::SSL OpenSSL::Crypto)