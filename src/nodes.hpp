#ifndef _MY_EXPR_NODES_
#define _MY_EXPR_NODES_

#include <concepts>
#include <stdfloat>

namespace calc{
    using evaluation_t = std::expected<num_t, calc_err>;

namespace{

    using token = tokenizer::token;

    struct expr{
        token tok;
        constexpr virtual evaluation_t evaluate() const = 0;
        constexpr virtual ~expr() = default;

        constexpr expr(token&& t): tok(std::move(t)){}
    };

    using expr_ptr_t = std::unique_ptr<expr>;

    struct binary_op : expr{
        using fun_t = evaluation_t(*)(num_t, num_t, const token&);

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

            return fun(*a, *b, tok);
        }

        static constexpr expr_ptr_t add(
            token&& t,
            expr_ptr_t&& l, 
            expr_ptr_t&& r
        ){
            return binary_op_with_fun(std::move(t), std::move(l), std::move(r), 
                [](num_t a, num_t b, const token& tok) constexpr -> evaluation_t {
                    auto ret = math_utils::safe_add(a, b);
                    if(ret){
                        return *ret;
                    }

                    return std::unexpected(
                        calc_err::error_with_wrong_token(
                            calc_err_type_t::OVERFLOW_UNDERFLOW, 
                            "overflow/underflow detected",
                            tok.full_expr,
                            tok.start,
                            tok.end
                        )
                    );
                }
            );       
        }

        static constexpr expr_ptr_t sub(
            token&& t,
            expr_ptr_t&& l, 
            expr_ptr_t&& r
        ){
            return binary_op_with_fun(std::move(t), std::move(l), std::move(r), 
                [](num_t a, num_t b, const token& tok) constexpr -> evaluation_t {
                    auto ret = math_utils::safe_sub(a, b);
                    if(ret){
                        return *ret;
                    }

                    return std::unexpected(
                        calc_err::error_with_wrong_token(
                            calc_err_type_t::OVERFLOW_UNDERFLOW, 
                            "overflow/underflow detected",
                            tok.full_expr,
                            tok.start,
                            tok.end
                        )
                    );
                }
            );   
        }

        static constexpr expr_ptr_t mult(
            token&& t,
            expr_ptr_t&& l, 
            expr_ptr_t&& r
        ){
            return binary_op_with_fun(std::move(t), std::move(l), std::move(r), 
                [](num_t a, num_t b, const token& tok) constexpr -> evaluation_t {
                    auto ret = math_utils::safe_mult(a, b);
                    if(ret){
                        return *ret;
                    }

                    return std::unexpected(
                        calc_err::error_with_wrong_token(
                            calc_err_type_t::OVERFLOW_UNDERFLOW, 
                            "overflow/underflow detected",
                            tok.full_expr,
                            tok.start,
                            tok.end
                        )
                    );
                }
            );   
        }

        static constexpr expr_ptr_t div(
            token&& t,
            expr_ptr_t&& l, 
            expr_ptr_t&& r
        ){
            return binary_op_with_fun(std::move(t), std::move(l), std::move(r), 
                [](num_t n, num_t d, const token& tok) constexpr -> evaluation_t {
                    if(math_utils::is_zero(d)){
                        return std::unexpected(
                            calc_err::error_with_wrong_token(
                                calc_err_type_t::DIVISION_BY_ZERO, 
                                "Division by 0 detected",
                                tok.full_expr,
                                tok.start,
                                tok.end
                            )
                        );
                    }

                    auto ret = math_utils::safe_div(n, d);
                    if(ret){
                        return *ret;
                    }

                    return std::unexpected(
                        calc_err::error_with_wrong_token(
                            calc_err_type_t::OVERFLOW_UNDERFLOW, 
                            "overflow/underflow detected",
                            tok.full_expr,
                            tok.start,
                            tok.end
                        )
                    );
                }
            );   
        }

        static constexpr expr_ptr_t exponent(
            token&& t,
            expr_ptr_t&& l, 
            expr_ptr_t&& r
        ){
            return binary_op_with_fun(std::move(t), std::move(l), std::move(r), 
                [](num_t b, num_t e, const token& tok) constexpr -> evaluation_t {
                    // auto ret = std::pow(b, e); constexpr since c++26
                    if(!math_utils::is_integer(e) || std::isless(e, 0)){
                        return std::unexpected(
                            calc_err::error_with_wrong_token(
                                calc_err_type_t::UNEXPECTED_VALUE,
                                "Exponent must be >=0 and integer",
                                tok.full_expr,
                                tok.start,
                                tok.end
                            )
                        );
                    }

                    if(math_utils::is_zero(b) || math_utils::equal(b, 1)){
                        return 1;
                    }

                    e = math_utils::remove_decimal_part(e);
                    
                    num_t ret = 1;
                    while(!math_utils::is_zero(e)){
                        auto tmp = math_utils::safe_mult(ret, b);
                        if(!tmp){
                            return std::unexpected(
                                calc_err::error_with_wrong_token(
                                    calc_err_type_t::OVERFLOW_UNDERFLOW, 
                                    "overflow/underflow detected",
                                    tok.full_expr,
                                    tok.start,
                                    tok.end
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
            token&& t,
            expr_ptr_t&& l, 
            expr_ptr_t&& r, 
            fun_t f
        ){
            return std::unique_ptr<binary_op>(
                new binary_op(
                    std::move(t),
                    std::move(l), 
                    std::move(r), 
                    f
                )
            );
        }

        constexpr binary_op(
            token&& t,
            expr_ptr_t&& l, 
            expr_ptr_t&& r, 
            fun_t f
        ) noexcept:
            expr::expr(std::move(t)),
            op1(std::move(l)), 
            op2(std::move(r)),
            fun(f)
        {}

    };

    struct unary_op : expr{
        using fun_t = evaluation_t(*)(num_t, const token& tok);
        
        expr_ptr_t data;
        fun_t fun;

        constexpr evaluation_t evaluate() const{
            auto ret = data->evaluate();
            if(!ret){
                return ret;
            }

            return fun(*ret, tok);
        }
        
        static constexpr expr_ptr_t neg(token&& t, expr_ptr_t&& data){
            return unary_op_with_fun(
                std::move(t),
                std::move(data), 
                [](num_t n, [[maybe_unused]] const token& tok) constexpr -> evaluation_t {
                    return -n;
                }
            );
        }

        static constexpr expr_ptr_t factorial(token&& t, expr_ptr_t&& data){
            using enum calc_err_type_t;
            return unary_op_with_fun(
                std::move(t),
                std::move(data), 
                [](num_t n, const token& tok) constexpr -> evaluation_t {

                    if(std::isless(n, 0)){
                        return std::unexpected(
                            calc_err::error_with_wrong_token(
                                calc_err_type_t::UNEXPECTED_VALUE, 
                                "Factorial can't be applied to a negative value",
                                tok.full_expr,
                                tok.start,
                                tok.end
                            )
                        );
                    }

                    n = math_utils::remove_decimal_part(n);

                    num_t ret = 1;
                    while(std::isgreater(n, 1.)){

                        auto tmp = math_utils::safe_mult(ret, n);
                        if(!tmp){
                            return std::unexpected(
                                calc_err::error_with_wrong_token(
                                    calc_err_type_t::OVERFLOW_UNDERFLOW,
                                    "overflow detected",
                                    tok.full_expr,
                                    tok.start,
                                    tok.end
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

        static constexpr expr_ptr_t abs(token&& t, expr_ptr_t&& data){
            using enum calc_err_type_t;
            return unary_op_with_fun(
                std::move(t),
                std::move(data), 
                [](num_t n, [[maybe_unused]] const token& tok) constexpr -> evaluation_t {
                    return std::fabs(n);
                }
            );
        }

        static constexpr expr_ptr_t floor(token&& t, expr_ptr_t&& data){
            using enum calc_err_type_t;
            return unary_op_with_fun(
                std::move(t),
                std::move(data), 
                [](num_t n, [[maybe_unused]] const token& tok) constexpr -> evaluation_t {
                    return std::floor(n);
                }
            );
        }

        static constexpr expr_ptr_t ceil(token&& t, expr_ptr_t&& data){
            using enum calc_err_type_t;
            return unary_op_with_fun(
                std::move(t),
                std::move(data), 
                [](num_t n, [[maybe_unused]] const token& tok) constexpr -> evaluation_t {
                    return std::ceil(n);
                }
            );
        }

    private:
        static constexpr expr_ptr_t unary_op_with_fun(
            token&& t,
            expr_ptr_t&& ptr, 
            fun_t f
        ){
            return std::unique_ptr<unary_op>(
                new unary_op(
                    std::move(t),
                    std::move(ptr), 
                    f
                )
            );
        }

        explicit constexpr unary_op(token&& t, expr_ptr_t&& ptr, fun_t f) noexcept: 
            expr::expr(std::move(t)),
            data(std::move(ptr)),
            fun(f)
        {}
    };

    struct lit : expr{
        num_t value;
        
        constexpr evaluation_t evaluate() const {
            return value;
        }

        static constexpr expr_ptr_t literal(token&& t, num_t n){
            return std::unique_ptr<lit>(
                new lit(
                    std::move(t), 
                    n
                )
            );
        }
        
    private:
        explicit constexpr lit(token&& t, num_t v) noexcept:
            expr::expr(std::move(t)), 
            value(v)
        {}
    };
}
}
#endif