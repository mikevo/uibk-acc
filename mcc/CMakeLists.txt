cmake_minimum_required(VERSION 2.8)
project (mcc)

set(PROJECT_NAME mcc)

include_directories(
    ../mC/
    include/
)

file(GLOB_RECURSE mcc_incs include/*.h)
file(GLOB_RECURSE mcc_srcs src/*.cpp)

set(EXECUTABLE_OUTPUT_PATH bin)

add_subdirectory(../libmc "../libmc")

add_executable(${PROJECT_NAME} ${mcc_srcs})
target_link_libraries (${PROJECT_NAME} mc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -g")


