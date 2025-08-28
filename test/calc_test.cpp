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
    static_assert(ev::evaluate<int>("1 + 1") == 2);
    static_assert(ev::evaluate<int>("1+1+1 +4") == 7);
    static_assert(ev::evaluate<int>("(1 + 1) + (2+ 1) + -10") == -5);
    static_assert(ev::evaluate<int>("(-1) + (-1 + -10 + (10 + 1))") == -1);
    static_assert(ev::evaluate<int>("-10 + -10") == -20);
    static_assert(ev::evaluate<int>("((((((((((((1+1)+1)+1)+1)+1)+1)+1)+1)+1)+1)+1)+1)+1") == 14);
    static_assert(ev::evaluate<int>("1+(1+(1+(1+(1+(1+(1+1))))))") == 8);
}

TEST(calc_test, sub){
    static_assert(ev::evaluate<int>("(2 + 3) - 1") == 4);
    static_assert(ev::evaluate<int>("1 + (2 - (3 + 4))") == -4);
    static_assert(ev::evaluate<int>("((10 + 5) - (3 + 2)) + 1") == 11);
    static_assert(ev::evaluate<int>("((1 + 2) + (3 + 4)) - (5 + 6)") == -1);
    static_assert(ev::evaluate<int>("(((1 + 1) + 1) + 1) - 2") == 2);
    static_assert(ev::evaluate<int>("(-5 + 2) - (-3)") == 0);
    static_assert(ev::evaluate<int>("(((((3 + 2))))) - 1") == 4);
    static_assert(ev::evaluate<int>("(0 - (1 + 2)) + 3") == 0);
    static_assert(ev::evaluate<int>("(100 - (50 + 25)) + (10 - 5)") == 30);
    static_assert(ev::evaluate<int>("(((7 + 3) - 4) + (2 - 1))") == 7);
}

TEST(calc_test, errors){
    using enum ev::calc_err_type_t;
   // static_assert(ev::evaluate<int>("1") == 1);
    static_assert(ev::evaluate<int>("1 + 1p").error().get_err_type() == UNKNOWN_TOKEN);

}