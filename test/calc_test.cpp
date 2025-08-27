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

TEST(calc_test, errors){
    using enum ev::calc_err_type_t;
   // static_assert(ev::evaluate<int>("1") == 1);
    static_assert(ev::evaluate<int>("1 + 1p").error().get_err_type() == UNKNOWN_TOKEN);

}