file(GLOB_RECURSE mcc_tac_incs include/*.h)
file(GLOB_RECURSE mcc_tac_srcs src/*.cpp)


add_library(mcctac STATIC ${mcc_tac_srcs} ${mcc_tac_incs})
target_link_libraries (mcctac mc mcccfg)

target_include_directories(mcctac PUBLIC
    include/
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -g")
