cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME mcc)

include_directories(
	./include/
)

set(HEADERS
	./include/ast.h
	./include/basics.h
	./include/parser.h
	./include/stream_utils.h
	./include/string_utils.h
	./include/test_utils.h
)

set(SOURCES
	./src/main.cpp
	./src/ast.cpp
	./src/parser.cpp
	./src/stream_utils.cpp
	./src/string_utils.cpp
)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_executable(${PROJECT_NAME}
    ${SOURCES} ${HEADERS}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -g")


