# additional target to perform clang-format run, requires clang-format

add_custom_target(
        format
        COMMAND /usr/bin/clang-format
        -i
        ${all_incs} ${all_srcs} ${all_tests}
)

