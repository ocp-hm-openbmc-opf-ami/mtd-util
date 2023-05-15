#
# Copyright (c) 2017-2022 Intel Corporation
#
# This software and the related documents are Intel copyrighted
# materials, and your use of them is governed by the express license
# under which they were provided to you ("License"). Unless the
# License provides otherwise, you may not use, modify, copy, publish,
# distribute, disclose or transmit this software or the related
# documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no
# express or implied warranties, other than those that are expressly
# stated in the License.
#

project(intel-fw-update CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

###############
# CMake options
cmake_minimum_required(VERSION 3.2)

###############
# build mtd emulation version
option(MTD_EMULATION "Enable MTD Emulation" OFF)
if (MTD_EMULATION)
    message(STATUS "Enabling MTD Emulation")
    add_definitions(-DMTD_EMULATION=1)
endif()

###############
# Developer Options (raw erase/write/cp commands)
option(DEVELOPER_OPTIONS "Enable developer options" OFF)
if (DEVELOPER_OPTIONS)
    message(STATUS "Enabling developer options")
    add_definitions(-DDEVELOPER_OPTIONS=1)
endif()

###############
# C++ options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -ffunction-sections -Wl,--gc-sections")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sysroot/include)

###############
# import Boost
find_package(Boost REQUIRED COMPONENTS iostreams)
find_package(Threads)
find_package(OpenSSL REQUIRED)

add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)
add_definitions(-DBOOST_SYSTEM_NO_DEPRECATED)
add_definitions(-DBOOST_ALL_NO_LIB)
find_package(Boost REQUIRED COMPONENTS iostreams)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(mtd-util "mtd-util.cpp" "debug.cpp" "mtd.cpp" "pfr.cpp")
target_link_libraries(mtd-util ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(mtd-util Boost::iostreams)
target_link_libraries(mtd-util OpenSSL::Crypto)
target_link_libraries(mtd-util gpiodcxx)

install (TARGETS mtd-util DESTINATION bin)

##############
# Tests
if(${BUILD_UT})

    ##############
    # import GTest
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})

    enable_testing()

    add_subdirectory(tests)

endif(${BUILD_UT})
