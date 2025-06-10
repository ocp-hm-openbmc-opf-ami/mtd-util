#
# Copyright (c) 2017-2025 Intel Corporation
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

project(intel-fw-update-tests CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

###############
# CMake options
cmake_minimum_required(VERSION 2.8)

###############
# C++ options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++20")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sysroot/include)

###############
# import Boost
find_package(Boost REQUIRED COMPONENTS iostreams)
add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)
add_definitions(-DBOOST_SYSTEM_NO_DEPRECATED)
add_definitions(-DBOOST_ALL_NO_LIB)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

###############
# import OpenSSL (crypto)
find_package(PkgConfig REQUIRED)
pkg_check_modules(Crypto crypto)
include_directories(${Crypto_INCLUDE_DIRS})
link_directories(${Crypto_LIBRARY_DIRS})

##############
# import GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

##############
# Tests
enable_testing()

# mtd-tests
add_executable(mtd-tests "mtd-tests.cpp" "../debug.cpp" "../mtd.cpp" "../pfr.cpp")
target_link_libraries(mtd-tests Boost::iostreams)
target_link_libraries(mtd-tests ${GTEST_BOTH_LIBRARIES} gmock)
target_link_libraries(mtd-tests pthread)
target_link_libraries(mtd-tests OpenSSL::Crypto)
target_link_libraries(mtd-tests gpiodcxx)

add_test(mtd-tests mtd-tests "--gtest_output=xml:${test_name}.xml")

# mtd-util-tests
add_executable(mtd-util-tests "mtd-util-tests.cpp" "../debug.cpp" "../mtd.cpp" "../pfr.cpp")
target_link_libraries(mtd-util-tests Boost::iostreams)
target_link_libraries(mtd-util-tests ${GTEST_BOTH_LIBRARIES} gmock)
target_link_libraries(mtd-util-tests pthread)
target_link_libraries(mtd-util-tests OpenSSL::Crypto)
target_link_libraries(mtd-utiltests gpiodcxx)
add_test(mtd-util-tests mtd-util-tests "--gtest_output=xml:${test_name}.xml")

