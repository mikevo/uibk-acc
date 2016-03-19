# Options. Turn on with 'cmake -Dmyvarname=ON'.
option(test "Build all tests." OFF) # Makes boolean 'test' available.

include_directories(
    include/
)

add_subdirectory(tac)

file(GLOB_RECURSE mcc_incs include/*.h)
file(GLOB_RECURSE mcc_srcs src/*.cpp)

set(EXECUTABLE_OUTPUT_PATH bin)

add_executable(${PROJECT_NAME} ${mcc_srcs} ${mcc_incs})
target_link_libraries (${PROJECT_NAME} mc mcctac)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -g")

################################
# Testing
################################
if (test)
    #enable_testing()
    
    set(UNIT_TESTS ${PROJECT_NAME}_tests)

    ##############
    # Unit Tests
    ##############
    file(GLOB_RECURSE mcc_tests test/*.cpp)
    add_executable(${UNIT_TESTS} ${mcc_tests})

    # Standard linking to gtest stuff.
    target_link_libraries(${UNIT_TESTS} gtest gtest_main)

    # Extra linking for the project.
    target_link_libraries(${UNIT_TESTS} mc mcctac)

    # This is so you can do 'make test' to see all your tests run, instead of
    # manually running the executable runUnitTests to see those specific tests.
    #add_test(NAME libmcTests COMMAND ${UNIT_TESTS})
    add_custom_target(unittests ${UNIT_TESTS})

endif()
