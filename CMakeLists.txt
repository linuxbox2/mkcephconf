
cmake_minimum_required (VERSION 2.8.11)
project (mkosd)

include(CheckCXXCompilerFlag)

set(CMAKE_CXX_COMPILER "/cache/llvm/bin/clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -g3 -gdwarf-4")

include_directories(
  ${PROJECT_SOURCE_DIR}/plustache/include
)

add_subdirectory(plustache)

set(mkosd_srcs
  mkosd_main.cc
  )

add_executable(mkosd ${mkosd_srcs})
target_link_libraries(mkosd
  plustache
  boost_filesystem
  boost_program_options
  boost_system)
