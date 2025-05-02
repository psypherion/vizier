cmake_minimum_required(VERSION 3.16)
project(vizier_engine)

# Use C++17, optimize, enable AVX2/BMI2
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -mavx2 -mbmi2 -Wall -Wextra")

# Include our headers
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)
