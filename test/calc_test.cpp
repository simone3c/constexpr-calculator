#include "gtest/gtest.h"

#include "parser.hpp"

TEST(calc_test, identity){
   // static_assert(ev::parser<int>("1").evaluate().value() == 1);
    EXPECT_EQ(ev::parser<int>("1").evaluate().value(), 1);
    EXPECT_EQ(ev::parser<int>("-1").evaluate().value(), -1);
    EXPECT_EQ(ev::parser<int>("(1)").evaluate().value(), 1);
    EXPECT_EQ(ev::parser<int>("(-1)").evaluate().value(), -1);
    EXPECT_EQ(ev::parser<int>("(((1)))").evaluate().value(), 1);
}

TEST(calc_test, sum){
   // static_assert(ev::parser<int>("1").evaluate().value() == 1);
    EXPECT_EQ(ev::parser<int>("1 + 1").evaluate().value(), 2);
    EXPECT_EQ(ev::parser<int>("1+1+1 +4").evaluate().value(), 7);
    EXPECT_EQ(ev::parser<int>("(1 + 1) + (2+ 1) + -10").evaluate().value(), -5);
    EXPECT_EQ(ev::parser<int>("(-1) + (-1 + -10 + (10 + 1))").evaluate().value(), -1);
    EXPECT_EQ(ev::parser<int>("-10 + -10").evaluate().value(), -20);
}