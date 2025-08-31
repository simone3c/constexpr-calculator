#ifndef _MY_EXPR_NODES_
#define _MY_EXPR_NODES_

#include <concepts>

namespace calc{

    template<typename num_t>
    requires std::is_arithmetic<num_t>::value
    using evaluation_t = std::expected<num_t, calc_err>;

namespace{

    template<typename num_t>
    requires std::is_arithmetic<num_t>::value
    struct expr{
        constexpr virtual evaluation_t<num_t> evaluate() const = 0;
        constexpr virtual ~expr() = default;
    };

    template<typename num_t>
    requires std::is_arithmetic<num_t>::value
    using expr_ptr_t = std::unique_ptr<expr<num_t>>;

    template<typename num_t>
    requires std::is_arithmetic<num_t>::value
    struct binary_op : expr<num_t>{
        using fun_t = evaluation_t<num_t>(*)(num_t, num_t);

        expr_ptr_t<num_t> op1;
        expr_ptr_t<num_t> op2;
        fun_t fun;

        constexpr evaluation_t<num_t> evaluate() const {
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

        static constexpr expr_ptr_t<num_t> add(
            expr_ptr_t<num_t>&& l, 
            expr_ptr_t<num_t>&& r
        ){
            return binary_op_with_fun(std::move(l), std::move(r), 
                [](num_t a, num_t b) constexpr -> evaluation_t<num_t> {
                    return a + b;
                }
            );       
        }

        static constexpr expr_ptr_t<num_t> sub(
            expr_ptr_t<num_t>&& l, 
            expr_ptr_t<num_t>&& r
        ){
            return binary_op_with_fun(std::move(l), std::move(r), 
                [](num_t a, num_t b) constexpr -> evaluation_t<num_t> {
                    return a - b;
                }
            );   
        }

        static constexpr expr_ptr_t<num_t> mult(
            expr_ptr_t<num_t>&& l, 
            expr_ptr_t<num_t>&& r
        ){
            return binary_op_with_fun(std::move(l), std::move(r), 
                [](num_t a, num_t b) constexpr -> evaluation_t<num_t> {
                    return a * b;
                }
            );   
        }

        static constexpr expr_ptr_t<num_t> div(
            expr_ptr_t<num_t>&& l, 
            expr_ptr_t<num_t>&& r
        ){
            return binary_op_with_fun(std::move(l), std::move(r), 
                [](num_t n, num_t d) constexpr -> evaluation_t<num_t> {
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

    private:
        static constexpr expr_ptr_t<num_t> binary_op_with_fun(
            expr_ptr_t<num_t>&& l, 
            expr_ptr_t<num_t>&& r, 
            fun_t f
        ){
            return std::unique_ptr<binary_op<num_t>>(
                new binary_op<num_t>(
                    std::move(l), 
                    std::move(r), 
                    f
                )
            );
        }

        constexpr binary_op(
            expr_ptr_t<num_t>&& l, 
            expr_ptr_t<num_t>&& r, 
            fun_t f
        ) noexcept:
            op1(std::move(l)), 
            op2(std::move(r)),
            fun(f)
        {}

    };

    template<typename num_t>
    requires std::is_arithmetic<num_t>::value
    struct unary_op : expr<num_t>{
        using fun_t = evaluation_t<num_t>(*)(num_t);
        
        expr_ptr_t<num_t> data;
        fun_t fun;

        constexpr evaluation_t<num_t> evaluate() const{
            auto ret = data->evaluate();
            if(!ret){
                return ret;
            }

            return fun(*ret);
        }
        
        static constexpr expr_ptr_t<num_t> neg(expr_ptr_t<num_t>&& data){
            return unary_op_with_fun(
                std::move(data), 
                [](num_t n) constexpr -> evaluation_t<num_t>{
                    return -n;
                }
            );
        }

        static constexpr expr_ptr_t<num_t> factorial(expr_ptr_t<num_t>&& data){
            using enum calc_err_type_t;
            return unary_op_with_fun(
                std::move(data), 
                [](num_t n) constexpr -> evaluation_t<num_t>{

                    if(std::is_floating_point<num_t>::value){
                        return std::unexpected(
                            calc_err::error_message(
                                calc_err_type_t::UNEXPECTED_VALUE, 
                                "Factorial can't be applied to a non-integer value"
                            )
                        );
                    }

                    if(n < math_utils::zero_element<num_t>()){
                        return std::unexpected(
                            calc_err::error_message(
                                calc_err_type_t::UNEXPECTED_VALUE, 
                                "Factorial can't be applied to a negative value"
                            )
                        );
                    }

                    num_t ret = 1;
                    while(n > 1){
                        ret *= n;
                        --n;
                    }

                    return ret;
                }
            );
        }

    private:
        static constexpr expr_ptr_t<num_t> unary_op_with_fun(
            expr_ptr_t<num_t>&& ptr, 
            fun_t f
        ){
            return std::unique_ptr<unary_op<num_t>>(
                new unary_op(
                    std::move(ptr), 
                    f
                )
            );
        }

        explicit constexpr unary_op(expr_ptr_t<num_t>&& ptr, fun_t f) noexcept: 
            data(std::move(ptr)),
            fun(f)
        {}
    };

    template<typename num_t>
    requires std::is_arithmetic<num_t>::value
    struct lit : expr<num_t>{
        num_t value;
        explicit constexpr lit(num_t v) noexcept: 
            value(v)
        {}

        constexpr evaluation_t<num_t> evaluate() const {
            return value;
        }
    };
}
}
#endif