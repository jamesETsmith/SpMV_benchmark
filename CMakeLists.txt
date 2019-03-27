cmake_minimum_required(VERSION 3.1)
project(SpMV)

#
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-g -O3")
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

#####################
# Find Dependencies #
#####################

# OpenMP
find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

# Eigen
if(DEFINED ENV{EIGEN3_INCLUDE_DIR})
  set(EIGEN3_INCLUDE_DIR $ENV{EIGEN3_INCLUDE_DIR})
message(STATUS "Eigen3 Include Dir. set to " ${EIGEN3_INCLUDE_DIR})
include_directories(${EIGEN3_INCLUDE_DIR})
else()
message(SEND_ERROR "You need to set the env variable EIGEN3_INCLUDE_DIR")
endif()

# Homewritten SpMV
add_executable(native ${SOURCE_DIR}/native.cpp)

# Eigen SpMV
add_executable(eigen_test ${SOURCE_DIR}/eigen_test.cpp)
