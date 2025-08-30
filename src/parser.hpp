#ifndef _MY_PARSER_
#define _MY_PARSER_

#include <memory>
#include <cassert>
#include <expected>
#include <type_traits>
#include <functional>
#include <cmath>
#include <functional>

#include "tokenizer.hpp"
#include "math_utils.hpp"

/*
    EXPR: MUL (('+' MUL_DIV)? | ('-' MUL_DIV)?)
    MUL_DIV: SIGN (('*' SIGN)? | ('/' SIGN)?)
    '^' ...
    SIGN: '-'? FACTORIAL
    FACTORIAL: ATOM '!'?
    ATOM: LIT | '(' EXPR ')' // SIN, LOG, ...
    LIT: int | double
*/

namespace calc{

    template<typename num_t>
    requires std::is_arithmetic<num_t>::value
    using evaluation_t = std::expected<num_t, calc_err>;

namespace {

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
                                "Factorial can't be applied to a non-interger value"
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
    
    template<typename num_t>
    requires std::is_arithmetic<num_t>::value
    class parser{
        
        expr_ptr_t<num_t> root;
        tokenizer t;
        std::string expr;

    public:
        constexpr parser() = default;

        constexpr std::expected<num_t, calc_err> evaluate(std::string_view str){
            using enum calc_err_type_t;

            calc_err err = parse(str);
            if(err.get_err_type() != NO_ERROR){
                return std::unexpected(err);
            }

            if(root == nullptr)
                return std::unexpected(
                    calc_err::error_message(
                        INVALID_EXPR, 
                        "Parsing failed"
                    )
                );

            return root->evaluate();
        }

    private:
        constexpr calc_err parse(std::string_view input){
            using enum calc_err_type_t;

            root.reset();
            expr.clear();

            expr = std::string{input};
            auto new_end = std::unique(std::begin(expr), std::end(expr), 
                [](char c1, char c2){
                    return c1 == c2 && (c1 == ' ' || c1 == '\t');
                }
            );
            expr.erase(new_end, std::end(expr));


            calc_err err = t.tokenize(expr);
            if(err.get_err_type() != NO_ERROR)
                return err;

            auto tmp = parse_exp();

            if(!tmp){
                return tmp.error();
            }

            if(t.has_more_tokens()){
                return calc_err::error_message(
                    UNEXPECTED_TOKEN, 
                    "Unexpected expression terminator found"
                );
            }

            root = std::move(*tmp);

            return calc_err::no_error();
        }

        constexpr std::expected<expr_ptr_t<num_t>, calc_err> parse_exp(){

            auto next_expr = parse_mul_div();
            if(!next_expr){
                return next_expr;
            }
            
            while(t.match(tokenizer::TOKEN_TYPE::PLUS) || 
                t.match(tokenizer::TOKEN_TYPE::MINUS)
            ){
                auto next = t.next();

                auto next_expr_2 = parse_mul_div();
                if(!next_expr_2){
                    return next_expr_2;
                }

                if(next->type == tokenizer::TOKEN_TYPE::PLUS){
                    next_expr = binary_op<num_t>::add(
                        std::move(*next_expr), 
                        std::move(*next_expr_2)
                    );    
                }
                else{
                    next_expr = binary_op<num_t>::sub(
                        std::move(*next_expr), 
                        std::move(*next_expr_2)
                    );  
                }
            }

            return next_expr;
        }

        constexpr std::expected<expr_ptr_t<num_t>, calc_err> parse_mul_div(){

            auto next_expr = parse_sign();
            if(!next_expr){
                return next_expr;
            }
            
            while(t.match(tokenizer::TOKEN_TYPE::ASTERISK) || 
                t.match(tokenizer::TOKEN_TYPE::SLASH)
            ){
                auto next = t.next();

                auto next_expr_2 = parse_sign();
                if(!next_expr_2){
                    return next_expr_2;
                }

                if(next->type == tokenizer::TOKEN_TYPE::ASTERISK){
                    next_expr = binary_op<num_t>::mult(
                        std::move(*next_expr), 
                        std::move(*next_expr_2)
                    ); 
                }
                else{
                    next_expr = binary_op<num_t>::div(
                        std::move(*next_expr), 
                        std::move(*next_expr_2)
                    ); 
                }
            }

            return next_expr;
        }

        constexpr std::expected<expr_ptr_t<num_t>, calc_err> parse_sign(){

            bool is_pos = true;

            if(t.match(tokenizer::TOKEN_TYPE::MINUS)){
                is_pos = false;
                t.next();
            }
            
            auto next_expr = parse_factorial();
            if(!next_expr || is_pos){
                return next_expr;
            }
            
            return unary_op<num_t>::neg(std::move(*next_expr));
        }
        
        constexpr std::expected<expr_ptr_t<num_t>, calc_err> parse_factorial(){
            
            auto next_expr = parse_atom();
            if(!next_expr){
                return next_expr;
            }

            if(t.match(tokenizer::TOKEN_TYPE::FACTORIAL)){
                t.next();
                return unary_op<num_t>::factorial(std::move(*next_expr));
            }
            
            return next_expr;
        }

        constexpr std::expected<expr_ptr_t<num_t>, calc_err> parse_atom(){
            using enum calc_err_type_t;

            std::expected<expr_ptr_t<num_t>, calc_err> tmp;
            std::optional<num_t> lit_val;

            auto tok = t.next();

            if(!tok){
                return std::unexpected(
                    calc_err::error_with_wrong_token(
                        EXPECTED_TOKEN, 
                        "Expected token, found end-of-expression instead", 
                        expr, 
                        expr.size() - 1, 
                        expr.size()
                    )
                );
            }
            
            switch (tok->type){
            case tokenizer::TOKEN_TYPE::OPEN_PAR:
                tmp = parse_exp();
                if(!tmp){
                    return tmp;
                }

                if(!t.consume(tokenizer::TOKEN_TYPE::CLOSED_PAR)){
                    return std::unexpected(
                        calc_err::error_with_wrong_token(
                            EXPECTED_TOKEN, 
                            "Expected a closed bracket ')'", 
                            expr, 
                            tok->start, 
                            tok->end
                        )
                    );
                }
                break;

            case tokenizer::TOKEN_TYPE::LIT:
                lit_val = lit_convert(tok->val);
                if(!lit_val){
                    return std::unexpected(
                        calc_err::error_with_wrong_token(
                            INVALID_LITERAL, 
                            "Invalid literal", 
                            expr, 
                            tok->start, 
                            tok->end
                        )
                    );
                }

                tmp = std::make_unique<lit<num_t>>(*lit_val); 
                
                break;

            // sin, cos, ...

            default:
                return std::unexpected(
                    calc_err::error_with_wrong_token(
                        INVALID_EXPR, 
                        "Invalid expression, expected a literal or function", 
                        expr, 
                        tok->start, 
                        tok->end
                    )
                );
            }

            return tmp;
        }

        constexpr std::optional<num_t> lit_convert(std::string_view n){
            num_t ret = 0;
            size_t i = 0;

            if(n.size() == 0){
                return std::nullopt;
            }

            auto decimal_point = std::find(std::begin(n), std::end(n), '.');

            for(auto ptr = std::begin(n); ptr != decimal_point; ++ptr){
                if(!(*ptr >= '0' && *ptr <= '9'))
                    return std::nullopt;

                ret *= 10;
                ret += (*ptr - '0');
                ++i;
            }

            if(!std::is_floating_point<num_t>::value || decimal_point == std::end(n)){
                return ret;
            }

            num_t div = 10;
            while(++decimal_point != std::end(n)){
                if(!(*decimal_point >= '0' && *decimal_point <= '9'))
                    return std::nullopt;

                ret += (*decimal_point - '0') / div;
                div *= 10;
            }

            return ret;
        }

    };
}

}

#endif