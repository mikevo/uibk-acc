cmake_minimum_required(VERSION 2.8)
project (mcc)
set(PROJECT_NAME mcc)

execute_process(COMMAND sh "./boost_install.sh")
set(Boost_INCLUDE_DIR "boost/")

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_subdirectory(libmc)
add_subdirectory(mcc)

include(cppcheck.cmake)
include(clang-format.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -g")

message(STATUS "Generarating ${CMAKE_CURRENT_SOURCE_DIR}/.clang_complete")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/.clang_complete "")
foreach(dir ${dirs})
  file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/.clang_complete "-I${dir}\n")
endforeach()
foreach(inc ${all_inc_dirs})
  file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/.clang_complete "-I${inc}\n")
endforeach()
string(REPLACE "'" "" CMAKE_CXX_FLAGS_SPLIT ${CMAKE_CXX_FLAGS})
string(REPLACE " " ";" CMAKE_CXX_FLAGS_SPLIT ${CMAKE_CXX_FLAGS_SPLIT})
foreach(flag ${CMAKE_CXX_FLAGS_SPLIT})
  file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/.clang_complete "${flag}\n")
endforeach()
