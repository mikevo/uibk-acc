cmake_minimum_required(VERSION 2.8)
project (mcc)

set(PROJECT_NAME mcc)

include_directories(
    ../mC/
    include/
)

file(GLOB_RECURSE mcc_incs include/*.h)
file(GLOB_RECURSE mcc_srcs src/*.cpp)

set(HEADERS ${mC_incs} ${mcc_incs})
set(SOURCES 
    ../mC/ast.cpp
    ../mC/parser.cpp
    ../mC/stream_utils.cpp
    ../mC/string_utils.cpp
    ${mcc_srcs}

)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_executable(${PROJECT_NAME}
    ${SOURCES} ${HEADERS}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -g")


