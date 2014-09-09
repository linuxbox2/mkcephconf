
cmake_minimum_required (VERSION 2.8.11)
project (mkcephconf)

include(CheckCXXCompilerFlag)

set(CMAKE_CXX_COMPILER "/cache/llvm/bin/clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -g3 -gdwarf-4")

include_directories(
  ${PROJECT_SOURCE_DIR}/plustache/include
)

add_subdirectory(plustache)

set(mkcephconf_srcs
  mkcephconf_main.cc
  )

add_executable(mkcephconf ${mkcephconf_srcs})
target_link_libraries(mkcephconf
  plustache
  boost_filesystem
  boost_program_options
  boost_regex
  boost_system)
