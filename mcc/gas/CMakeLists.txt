file(GLOB_RECURSE mcc_gas_incs include/*.h)
file(GLOB_RECURSE mcc_gas_srcs src/*.cpp)

set(all_incs ${all_incs} ${mcc_gas_incs} PARENT_SCOPE)
set(all_srcs ${all_srcs} ${mcc_gas_srcs} PARENT_SCOPE)

set(all_inc_dirs ${all_inc_dirs} "${CMAKE_CURRENT_SOURCE_DIR}/include/" PARENT_SCOPE)

add_library(mccgas STATIC ${mcc_gas_srcs} ${mcc_gas_incs})
target_link_libraries (mccgas mc mcccfg)

target_include_directories(mccgas PUBLIC
    include/
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -g")
