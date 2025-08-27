#include "gtest/gtest.h"

#include "parser.hpp"

TEST(calc_test, identity){
    static_assert(ev::evaluate<int>("1") == 1);
    static_assert(ev::evaluate<int>("-1") == -1);
    static_assert(ev::evaluate<int>("(1)") == 1);
    static_assert(ev::evaluate<int>("(-1)") == -1);
    static_assert(ev::evaluate<int>("(((1)))") == 1);
}

TEST(calc_test, sum){
   // static_assert(ev::evaluate<int>("1") == 1);
    static_assert(ev::evaluate<int>("1 + 1") == 2);
    static_assert(ev::evaluate<int>("1+1+1 +4") == 7);
    static_assert(ev::evaluate<int>("(1 + 1) + (2+ 1) + -10") == -5);
    static_assert(ev::evaluate<int>("(-1) + (-1 + -10 + (10 + 1))") == -1);
    static_assert(ev::evaluate<int>("-10 + -10") == -20);
}