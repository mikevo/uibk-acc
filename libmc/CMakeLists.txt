set(SOURCES 
    ./src/ast.cpp
    ./src/parser.cpp
    ./src/stream_utils.cpp
    ./src/string_utils.cpp
    ./include/ast.h
    ./include/basics.h
    ./include/parser.h
    ./include/stream_utils.h
    ./include/string_utils.h
    ./include/test_utils.h
)

add_library(mc STATIC ${SOURCES})

target_include_directories(mc PUBLIC
    include/
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -g")
