#ifndef _MY_EXPR_NODES_
#define _MY_EXPR_NODES_

#include <concepts>
#include <stdfloat>

namespace calc{
    using evaluation_t = std::expected<num_t, calc_err>;

namespace{

    struct expr{
        constexpr virtual evaluation_t evaluate() const = 0;
        constexpr virtual ~expr() = default;
    };

    using expr_ptr_t = std::unique_ptr<expr>;

    struct binary_op : expr{
        using fun_t = evaluation_t(*)(num_t, num_t);

        expr_ptr_t op1;
        expr_ptr_t op2;
        fun_t fun;

        constexpr evaluation_t evaluate() const {
            auto a = op1->evaluate();
            if(!a){
                return a;
            }

            auto b = op2->evaluate();
            if(!b){
                return b;
            }

            return fun(*a, *b);
        }

        static constexpr expr_ptr_t add(
            expr_ptr_t&& l, 
            expr_ptr_t&& r
        ){
            return binary_op_with_fun(std::move(l), std::move(r), 
                [](num_t a, num_t b) constexpr -> evaluation_t {
                    auto ret = math_utils::safe_add(a, b);
                    if(ret){
                        return *ret;
                    }

                    return std::unexpected(
                        calc_err::error_message(
                            calc_err_type_t::OVERFLOW_UNDERFLOW, 
                            "overflow/underflow detected"
                        )
                    );
                }
            );       
        }

        static constexpr expr_ptr_t sub(
            expr_ptr_t&& l, 
            expr_ptr_t&& r
        ){
            return binary_op_with_fun(std::move(l), std::move(r), 
                [](num_t a, num_t b) constexpr -> evaluation_t {
                    auto ret = math_utils::safe_sub(a, b);
                    if(ret){
                        return *ret;
                    }

                    return std::unexpected(
                        calc_err::error_message(
                            calc_err_type_t::OVERFLOW_UNDERFLOW, 
                            "overflow/underflow detected"
                        )
                    );
                }
            );   
        }

        static constexpr expr_ptr_t mult(
            expr_ptr_t&& l, 
            expr_ptr_t&& r
        ){
            return binary_op_with_fun(std::move(l), std::move(r), 
                [](num_t a, num_t b) constexpr -> evaluation_t {
                    auto ret = math_utils::safe_mult(a, b);
                    if(ret){
                        return *ret;
                    }

                    return std::unexpected(
                        calc_err::error_message(
                            calc_err_type_t::OVERFLOW_UNDERFLOW, 
                            "overflow/underflow detected"
                        )
                    );
                }
            );   
        }

        // TODO handle floating point issues for divison
        static constexpr expr_ptr_t div(
            expr_ptr_t&& l, 
            expr_ptr_t&& r
        ){
            return binary_op_with_fun(std::move(l), std::move(r), 
                [](num_t n, num_t d) constexpr -> evaluation_t {
                    if(math_utils::is_zero(d)){
                        return std::unexpected(
                            calc_err::error_message(
                                calc_err_type_t::DIVISION_BY_ZERO, 
                                "Division by 0 detected"
                            )
                        );
                    }

                    return n / d;
                }
            );   
        }

        static constexpr expr_ptr_t exponent(
            expr_ptr_t&& l, 
            expr_ptr_t&& r
        ){
            return binary_op_with_fun(std::move(l), std::move(r), 
                [](num_t b, num_t e) constexpr -> evaluation_t {
                    // auto ret = std::pow(b, e); constexpr since c++26
                    if(!math_utils::is_integer(e) || std::isless(e, 0)){
                        return std::unexpected(
                            calc_err::error_message(
                                calc_err_type_t::UNEXPECTED_VALUE,
                                "Exponent may be <0 or non integer"
                            )
                        );
                    }

                    e = math_utils::remove_decimal_part(e);
                    
                    num_t ret = 1;
                    while(!math_utils::is_zero(e)){
                        auto tmp = math_utils::safe_mult(ret, b);
                        if(!tmp){
                            return std::unexpected(
                                calc_err::error_message(
                                    calc_err_type_t::OVERFLOW_UNDERFLOW, 
                                    "overflow/underflow detected"
                                )
                            );
                        }
                        ret = *tmp;
                        --e;
                    }

                    return ret;
                }
            );   
        }

    private:
        static constexpr expr_ptr_t binary_op_with_fun(
            expr_ptr_t&& l, 
            expr_ptr_t&& r, 
            fun_t f
        ){
            return std::unique_ptr<binary_op>(
                new binary_op(
                    std::move(l), 
                    std::move(r), 
                    f
                )
            );
        }

        constexpr binary_op(
            expr_ptr_t&& l, 
            expr_ptr_t&& r, 
            fun_t f
        ) noexcept:
            op1(std::move(l)), 
            op2(std::move(r)),
            fun(f)
        {}

    };

    struct unary_op : expr{
        using fun_t = evaluation_t(*)(num_t);
        
        expr_ptr_t data;
        fun_t fun;

        constexpr evaluation_t evaluate() const{
            auto ret = data->evaluate();
            if(!ret){
                return ret;
            }

            return fun(*ret);
        }
        
        static constexpr expr_ptr_t neg(expr_ptr_t&& data){
            return unary_op_with_fun(
                std::move(data), 
                [](num_t n) constexpr -> evaluation_t {
                    return -n;
                }
            );
        }

        static constexpr expr_ptr_t factorial(expr_ptr_t&& data){
            using enum calc_err_type_t;
            return unary_op_with_fun(
                std::move(data), 
                [](num_t n) constexpr -> evaluation_t {

                    if(std::isless(n, 0)){
                        return std::unexpected(
                            calc_err::error_message(
                                calc_err_type_t::UNEXPECTED_VALUE, 
                                "Factorial can't be applied to a negative value"
                            )
                        );
                    }

                    n = math_utils::remove_decimal_part(n);

                    num_t ret = 1;
                    while(std::isgreater(n, 1.)){

                        auto tmp = math_utils::safe_mult(ret, n);
                        if(!tmp){
                            return std::unexpected(
                                calc_err::error_message(
                                    calc_err_type_t::OVERFLOW_UNDERFLOW,
                                    "overflow detected"
                                )
                            );
                        }
                        ret = *tmp;

                        --n;
                    }

                    return ret;
                }
            );
        }

    private:
        static constexpr expr_ptr_t unary_op_with_fun(
            expr_ptr_t&& ptr, 
            fun_t f
        ){
            return std::unique_ptr<unary_op>(
                new unary_op(
                    std::move(ptr), 
                    f
                )
            );
        }

        explicit constexpr unary_op(expr_ptr_t&& ptr, fun_t f) noexcept: 
            data(std::move(ptr)),
            fun(f)
        {}
    };

    template<typename num_t>
    requires std::is_signed<num_t>::value
    struct lit : expr{
        num_t value;
        explicit constexpr lit(num_t v) noexcept: 
            value(v)
        {}

        constexpr evaluation_t evaluate() const {
            return value;
        }
    };
}
}
#endif