file(GLOB_RECURSE mcc_lvn_incs include/*.h)
file(GLOB_RECURSE mcc_lvn_srcs src/*.cpp)


add_library(mcclvn STATIC ${mcc_lvn_srcs} ${mcc_lvn_incs})
target_link_libraries (mcclvn mcctac mc)

target_include_directories(mcclvn PUBLIC
    include/
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -g")
