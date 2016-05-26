file(GLOB_RECURSE mcc_cfg_incs include/*.h)
file(GLOB_RECURSE mcc_cfg_srcs src/*.cpp)

set(all_incs ${all_incs} ${mcc_cfg_incs} PARENT_SCOPE)
set(all_srcs ${all_srcs} ${mcc_cfg_srcs} PARENT_SCOPE)

set(all_inc_dirs ${all_inc_dirs} "${CMAKE_CURRENT_SOURCE_DIR}/include/" PARENT_SCOPE)

add_library(mcccfg STATIC ${mcc_cfg_srcs} ${mcc_cfg_incs})
target_link_libraries (mcccfg mcctac mc)

target_include_directories(mcccfg PUBLIC
    include/
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -g")
