
cmake_minimum_required (VERSION 2.8.11)
project (mkosd)

include(CheckCXXCompilerFlag)

set(CMAKE_CXX_COMPILER "/cache/llvm/bin/clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


set(mkosd_srcs
  mkosd_main.cc
  )

add_executable(mkosd ${mkosd_srcs})
