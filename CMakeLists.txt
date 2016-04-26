cmake_minimum_required(VERSION 2.8)
project (mcc)
set(PROJECT_NAME mcc)

execute_process(COMMAND sh "./boost_install.sh")
set(Boost_INCLUDE_DIR "boost/")

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_subdirectory(libmc)
add_subdirectory(mcc)

include(cppcheck.cmake)
include(clang-format.cmake)

