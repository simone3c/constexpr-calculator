#include "gtest/gtest.h"

#include "constexpr-calculator/calculator.hpp"

TEST(calc_test, identity){
    static_assert(calc::evaluate("1") == 1);
    static_assert(calc::evaluate("-1") == -1);
    static_assert(calc::evaluate("(1)") == 1.);
    static_assert(calc::evaluate("(-1)") == -1.);
    static_assert(calc::evaluate("(((1)))") == 1);
    static_assert(calc::evaluate("-(((1)))") == -1);
    static_assert(calc::evaluate("-((-(1)))") == 1);
    static_assert(calc::evaluate("1.5") == 1.5);
    static_assert(calc::evaluate(".5") == 0.5);
    static_assert(calc::evaluate("10") == 10);
    static_assert(calc::evaluate("0") == 0);
    static_assert(calc::evaluate("-.25") == -0.25);

}

TEST(calc_test, sum){
    static_assert(calc::evaluate("1 + 1") == 2);
    static_assert(calc::evaluate("1+1+1 +4") == 7);
    static_assert(calc::evaluate("(1 + 1) + (2+ 1) + -10") == -5);
    static_assert(calc::evaluate("(-1) + (-1 + -10 + (10 + 1))") == -1);
    static_assert(calc::evaluate("-10 + -10") == -20);
    static_assert(calc::evaluate("((((((((((((1+1)+1)+1)+1)+1)+1)+1)+1)+1)+1)+1)+1)+1") == 14);
    static_assert(calc::evaluate("1+(1+(1+(1+(1+(1+(1+1))))))") == 8);
}

TEST(calc_test, sub){
    static_assert(calc::evaluate("(2 + 3) - 1") == 4);
    static_assert(calc::evaluate("1 + (2 - (3 + 4))") == -4);
    static_assert(calc::evaluate("((10 + 5) - (3 + 2)) + 1") == 11);
    static_assert(calc::evaluate("((1 + 2) + (3 + 4)) - (5 + 6)") == -1);
    static_assert(calc::evaluate("(((1 + 1) + 1) + 1) - 2") == 2);
    static_assert(calc::evaluate("(-5 + 2) - (-3)") == 0);
    static_assert(calc::evaluate("(((((3 + 2))))) - 1") == 4);
    static_assert(calc::evaluate("(0 - (1 + 2)) + 3") == 0);
    static_assert(calc::evaluate("(100 - (50 + 25)) + (10 - 5)") == 30);
    static_assert(calc::evaluate("(((7 + 3) - 4) + (2 - 1))") == 7);
}

TEST(calc_test, mult){
    static_assert(calc::evaluate("2 * 3") == 6);
    static_assert(calc::evaluate("1 * 4 * 5") == 20);
    static_assert(calc::evaluate("-2 * 3") == -6);
    static_assert(calc::evaluate("(2 * 2) * 3") == 12);
    static_assert(calc::evaluate("1 * (2 * 3 * 4)") == 24);
    static_assert(calc::evaluate("(1 * 2) * (3 * 4)") == 24);
    static_assert(calc::evaluate("-1 * (2 * (3 * 4))") == -24);
    static_assert(calc::evaluate("(-1) * ((2 * 2) * (2 * 2))") == -16);
    static_assert(calc::evaluate("(1 * 2 * 3 * 4 * 5)") == 120);
    static_assert(calc::evaluate("((2 * 3) * (4 * 5)) * (1 * 1)") == 120);
}

TEST(calc_test, div){
    static_assert(calc::evaluate("6 / 2") == 3);
    static_assert(calc::evaluate("8 / 2 / 2") == 2);
    static_assert(calc::evaluate("-8 / 2") == -4);
    static_assert(calc::evaluate("(16 / 4) / 2") == 2);
    static_assert(calc::evaluate("(100 / 5) / (2)") == 10);
    static_assert(calc::evaluate("(64 / 2) / (2 / 1)") == 16);
    static_assert(calc::evaluate("-64 / (4 / 2)") == -32);
    static_assert(calc::evaluate("((64 / 2) / 2) / 2") == 8);
    static_assert(calc::evaluate("((256 / 2) / 2) / ((2 / 1))") == 32);
    static_assert(calc::evaluate("((((1024 / 2) / 2) / 2) / 2)") == 64);
    static_assert(calc::math_utils::equal(*calc::evaluate("(1 + 2) * (3 + 4) / 5"), 4.2));
    static_assert(calc::math_utils::equal(*calc::evaluate("((5 + 5) * (3 - 1) + 6) / 4"), 6.5));
    static_assert(calc::math_utils::equal(*calc::evaluate("1 / 3"), .33333333333333));
    
}

TEST(calc_test, factorial){
    static_assert(calc::evaluate("(5 + 3)! / 2!") == 20160);
    static_assert(calc::evaluate("(4 * 3)! / (2 * 3)!" ) == 665280);
    static_assert(calc::evaluate("(5 + 3) * 2!") == 16);
    static_assert(calc::evaluate("(3! + 2!) * 2!") == 16);
    static_assert(calc::evaluate("(6 / 3)! * 2!") == 4);
    static_assert(calc::evaluate("(5 * 2)! / (3! * 2!)") == 302400);
    static_assert(calc::evaluate("(4! / 2!) * 3!") == 72);
    static_assert(calc::evaluate("(5! / 2!) * 3!") == 360);
    static_assert(calc::evaluate("(5 + 3) * (2! / 1!)") == 16);
    static_assert(calc::evaluate("(6 + 2) * (3! / 2!)") == 24);
}

TEST(calc_test, exponent){
    static_assert(calc::evaluate("(3 + 1)! - 2 ^ 3") == 16);
    static_assert(calc::evaluate("(2 + 2)! + (3 ^ 2)") == 33);
    static_assert(calc::evaluate("(5 - 2)! ^ 2") == 36);
    static_assert(calc::evaluate("(2 + 1)! + (2 ^ (2 + 1))") == 14);
    static_assert(calc::evaluate("(4 + 1)! - (3 + 3)") == 114);
    static_assert(calc::evaluate("(6 - 3)! ^ (1 + 1)") == 36);
    static_assert(calc::evaluate("(2 + 3 ^ 1)! - (2 + 2)!") == 96);
    static_assert(calc::evaluate("(3 + 1) ^ 2 + 3!") == 22);
    static_assert(calc::evaluate("-((2 + 1)! + 1) ^ 2") == 49);
    static_assert(calc::evaluate("(2 + 3)! - ((1 + 1) ^ 3)") == 112);
    static_assert(calc::evaluate("(5 + 1)! ^ 0 + 3") == 4);


}

TEST(calc_test, syntax_errors){
    using enum calc::calc_err_type_t;

    static_assert(calc::evaluate("").error().get_err_type() == EMPTY_EXPRESSION);

    static_assert(calc::evaluate("1 + 1p").error().get_err_type() == UNKNOWN_TOKEN);

    static_assert(calc::evaluate("(3+4").error().get_err_type() == EXPECTED_TOKEN);
    static_assert(calc::evaluate("1+").error().get_err_type() == EXPECTED_TOKEN);
    static_assert(calc::evaluate("1 1+1").error().get_err_type() == UNEXPECTED_TOKEN);
}

TEST(calc_test, evaluation_errors){
    using enum calc::calc_err_type_t;

    static_assert(calc::evaluate("10 / 0").error().get_err_type() == DIVISION_BY_ZERO);
    static_assert(calc::evaluate("10 / (1-1)").error().get_err_type() == DIVISION_BY_ZERO);

    static_assert(calc::evaluate("(-1)!").error().get_err_type() == UNEXPECTED_VALUE);

    EXPECT_EQ(calc::evaluate("10000000!").error().get_err_type(), OVERFLOW_UNDERFLOW);
    EXPECT_EQ(calc::evaluate("10000^100000000").error().get_err_type(), OVERFLOW_UNDERFLOW);
    EXPECT_EQ(calc::evaluate("10000^1000 * 100000000000000000").error().get_err_type(), OVERFLOW_UNDERFLOW);

    static_assert(calc::evaluate("2^-5").error().get_err_type() == UNEXPECTED_VALUE);

}