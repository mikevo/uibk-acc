file(GLOB_RECURSE mcc_cfg_incs include/*.h)
file(GLOB_RECURSE mcc_cfg_srcs src/*.cpp)


add_library(mcccfg STATIC ${mcc_cfg_srcs} ${mcc_cfg_incs})
target_link_libraries (mcccfg mcctac mc)

target_include_directories(mcccfg PUBLIC
    include/
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -g")
