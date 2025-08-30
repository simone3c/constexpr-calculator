#include "gtest/gtest.h"

#include "constexpr-calculator/calculator.hpp"

TEST(calc_test, identity){
    static_assert(calc::evaluate<int>("1") == 1);
    static_assert(calc::evaluate<int>("-1") == -1);
    static_assert(calc::evaluate<int>("(1)") == 1);
    static_assert(calc::evaluate<int>("(-1)") == -1);
    static_assert(calc::evaluate<int>("(((1)))") == 1);
    static_assert(calc::evaluate<int>("-(((1)))") == -1);
    static_assert(calc::evaluate<int>("-((-(1)))") == 1);
    static_assert(calc::evaluate<int>("1.5") == 1);
    static_assert(calc::evaluate<int>("-.5") == 0);
    static_assert(calc::evaluate<int>("10.0") == 10);
    static_assert(calc::evaluate<int>("0.0") == 0);
    static_assert(calc::evaluate<int>(".25") == 0);
    static_assert(calc::evaluate<double>("1.5") == 1.5);
    static_assert(calc::evaluate<double>(".5") == 0.5);
    static_assert(calc::evaluate<double>("10.0") == 10.0);
    static_assert(calc::evaluate<double>("0.0") == 0.0);
    static_assert(calc::evaluate<double>("-.25") == -0.25);

}

TEST(calc_test, sum){
    static_assert(calc::evaluate<int>("1 + 1") == 2);
    static_assert(calc::evaluate<int>("1+1+1 +4") == 7);
    static_assert(calc::evaluate<int>("(1 + 1) + (2+ 1) + -10") == -5);
    static_assert(calc::evaluate<int>("(-1) + (-1 + -10 + (10 + 1))") == -1);
    static_assert(calc::evaluate<int>("-10 + -10") == -20);
    static_assert(calc::evaluate<int>("((((((((((((1+1)+1)+1)+1)+1)+1)+1)+1)+1)+1)+1)+1)+1") == 14);
    static_assert(calc::evaluate<int>("1+(1+(1+(1+(1+(1+(1+1))))))") == 8);
}

TEST(calc_test, sub){
    static_assert(calc::evaluate<int>("(2 + 3) - 1") == 4);
    static_assert(calc::evaluate<int>("1 + (2 - (3 + 4))") == -4);
    static_assert(calc::evaluate<int>("((10 + 5) - (3 + 2)) + 1") == 11);
    static_assert(calc::evaluate<int>("((1 + 2) + (3 + 4)) - (5 + 6)") == -1);
    static_assert(calc::evaluate<int>("(((1 + 1) + 1) + 1) - 2") == 2);
    static_assert(calc::evaluate<int>("(-5 + 2) - (-3)") == 0);
    static_assert(calc::evaluate<int>("(((((3 + 2))))) - 1") == 4);
    static_assert(calc::evaluate<int>("(0 - (1 + 2)) + 3") == 0);
    static_assert(calc::evaluate<int>("(100 - (50 + 25)) + (10 - 5)") == 30);
    static_assert(calc::evaluate<int>("(((7 + 3) - 4) + (2 - 1))") == 7);
}

TEST(calc_test, mult){
    static_assert(calc::evaluate<int>("2 * 3") == 6);
    static_assert(calc::evaluate<int>("1 * 4 * 5") == 20);
    static_assert(calc::evaluate<int>("-2 * 3") == -6);
    static_assert(calc::evaluate<int>("(2 * 2) * 3") == 12);
    static_assert(calc::evaluate<int>("1 * (2 * 3 * 4)") == 24);
    static_assert(calc::evaluate<int>("(1 * 2) * (3 * 4)") == 24);
    static_assert(calc::evaluate<int>("-1 * (2 * (3 * 4))") == -24);
    static_assert(calc::evaluate<int>("(-1) * ((2 * 2) * (2 * 2))") == -16);
    static_assert(calc::evaluate<int>("(1 * 2 * 3 * 4 * 5)") == 120);
    static_assert(calc::evaluate<int>("((2 * 3) * (4 * 5)) * (1 * 1)") == 120);
}

TEST(calc_test, div_truncated){
    static_assert(calc::evaluate<int>("6 / 2") == 3);
    static_assert(calc::evaluate<int>("8 / 2 / 2") == 2);
    static_assert(calc::evaluate<int>("-8 / 2") == -4);
    static_assert(calc::evaluate<int>("(16 / 4) / 2") == 2);
    static_assert(calc::evaluate<int>("(100 / 5) / (2)") == 10);
    static_assert(calc::evaluate<int>("(64 / 2) / (2 / 1)") == 16);
    static_assert(calc::evaluate<int>("-64 / (4 / 2)") == -32);
    static_assert(calc::evaluate<int>("((64 / 2) / 2) / 2") == 8);
    static_assert(calc::evaluate<int>("((256 / 2) / 2) / ((2 / 1))") == 32);
    static_assert(calc::evaluate<int>("((((1024 / 2) / 2) / 2) / 2)") == 64);
}

TEST(calc_test, basic_operands_not_truncated){
    static_assert(calc::math_utils::equal(*calc::evaluate<double>("(1 + 2) * (3 + 4) / 5"), 4.2));
    static_assert(calc::math_utils::equal(*calc::evaluate<double>("((8 - 3) * (2 + 1)) + (4 / 2)"), 17.0));
    static_assert(calc::math_utils::equal(*calc::evaluate<double>("(18 + (12 / 3)) * (2 - 1)"), 22.0));
    static_assert(calc::math_utils::equal(*calc::evaluate<double>("((6 * 3) - (4 + 2)) / 2"), 6.0));
    static_assert(calc::math_utils::equal(*calc::evaluate<double>("-((10 - 3) * (2 + 2)) + 5"), -23.0));
    static_assert(calc::math_utils::equal(*calc::evaluate<double>("((5 + 5) * (3 - 1) + 6) / 4"), 6.5));
    static_assert(calc::math_utils::equal(*calc::evaluate<double>("(20 / (2 + 3)) + (6 * 2)"), 16.0));
    static_assert(calc::math_utils::equal(*calc::evaluate<double>("(((9 + 3) / 3) * 2) - 1"), 7.0));
    static_assert(calc::math_utils::equal(*calc::evaluate<double>("((15 - 5) / (2 + 3)) + (1 * 4)"), 6.0));
    static_assert(calc::math_utils::equal(*calc::evaluate<double>("(((((2 + 2) * 2) + 4) / 2) - 1) * 3"), 15.0));
}

TEST(calc_test, errors){
    using enum calc::calc_err_type_t;
    static_assert(calc::evaluate<int>("1 + 1p").error().get_err_type() == UNKNOWN_TOKEN);
    static_assert(calc::evaluate<int>("10 / 0").error().get_err_type() == DIVISION_BY_ZERO);
    static_assert(calc::evaluate<int>("10 / (1-1)").error().get_err_type() == DIVISION_BY_ZERO);
    static_assert(calc::evaluate<int>("10 / (0 / 1)").error().get_err_type() == DIVISION_BY_ZERO);
}