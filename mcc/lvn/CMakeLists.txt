file(GLOB_RECURSE mcc_lvn_incs include/*.h)
file(GLOB_RECURSE mcc_lvn_srcs src/*.cpp)

set(all_incs ${all_incs} ${mcc_lvn_incs} PARENT_SCOPE)
set(all_srcs ${all_srcs} ${mcc_lvn_srcs} PARENT_SCOPE)

set(all_inc_dirs ${all_inc_dirs} "${CMAKE_CURRENT_SOURCE_DIR}/include/" PARENT_SCOPE)

add_library(mcclvn STATIC ${mcc_lvn_srcs} ${mcc_lvn_incs})
target_link_libraries (mcclvn mcctac mc)

target_include_directories(mcclvn PUBLIC
    include/
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -g")
