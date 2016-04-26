# additional target to perform cppcheck run, requires cppcheck

set(all_files ${all_incs} ${all_srcs} ${all_tests})

add_custom_target(
        cppcheck
        COMMAND /usr/bin/cppcheck
        --suppress=missingInclude
        --enable=warning,performance,portability,information
        --std=c++11
        --verbose
        --quiet
        ${all_files}
)
