#include <gtest/gtest.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "ast.h"
#include "parser.h"
#include "test_utils.h"
#include "stream_utils.h"

TEST(External, Types) {
    string str_plus{"+"};
    string str_test{"test"};
    string str_int{"int"};
    string str_float{"  float "};
    
    EXPECT_NO_MATCH(parser::type, str_plus);
    EXPECT_NO_MATCH(parser::type, str_test);
    EXPECT_MATCH_TYPED(parser::type, str_int, ast::int_type, [](auto res) {
        return true;
    });
    EXPECT_MATCH_TYPED(parser::type, str_float, ast::float_type, [](auto res) {
        return true;
    });
}



