# Copyright (c) 2016 Andrew Sutton
# All rights reserved

cmake_minimum_required(VERSION 3.0)

project(stl2 CXX)

set(CMAKE_CXX_FLAGS "-std=c++17")

enable_testing()

add_library(stl
  std/traits.cpp
  std/concepts.cpp
  std/utility.cpp
  std/functional.cpp
  std/iterator.cpp
  std/range.cpp
  std/algorithm.cpp)


include_directories(.)


macro(add_unit_test target)
  add_executable(${target} ${ARGN})
  target_link_libraries(${target} stl)
  add_test(${target}_t ${target})
endmacro()

add_unit_test(test_concepts test/concepts.cpp)
add_unit_test(test_functional test/functional.cpp)
add_unit_test(test_iterator test/iterator.cpp)
