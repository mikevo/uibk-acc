include_directories(
    ../mC/
)

set(SOURCES 
    ../mC/ast.cpp
    ../mC/parser.cpp
    ../mC/stream_utils.cpp
    ../mC/string_utils.cpp

)

add_library(mc ${SOURCES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -g")
