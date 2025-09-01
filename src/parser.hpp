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
#include "nodes.hpp"

/*
    EXPR: MUL_DIV (('+' MUL_DIV)? | ('-' MUL_DIV)?)
    MUL_DIV: EXPONENT (('*' EXPONENT)? | ('/' EXPONENT)?)
    EXPONENT: SIGN ('^' SIGN)?
    SIGN: '-'? FACTORIAL
    FACTORIAL: ATOM '!'?
    ATOM: LIT | '(' EXPR ')' // SIN, LOG, ...
    LIT: int | double
*/

namespace calc{
namespace {
    
    class parser{
        
        expr_ptr_t root;
        tokenizer t;
        std::string expr;

    public:
        constexpr parser() = default;

        constexpr std::expected<num_t, calc_err> evaluate(std::string_view str){
            using enum calc_err_type_t;

            auto err = parse(str);

            if(err){
                return std::unexpected(*err);
            }

            assert(root != nullptr);
            
            return root->evaluate();
        }

    private:
        constexpr std::optional<calc_err> parse(std::string_view input){
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


            auto err = t.tokenize(expr);
            if(err){
                return err;
            }

            auto tmp = parse_exp();

            if(!tmp){
                return tmp.error();
            }

            if(t.has_more_tokens()){
                auto tok = t.next();
                return calc_err::error_with_wrong_token(
                    UNEXPECTED_TOKEN, 
                    "Unexpected token after end-of-expression",
                    expr,
                    tok->start,
                    tok->end
                );
            }

            root = std::move(*tmp);

            return std::nullopt;
        }

        constexpr std::expected<expr_ptr_t, calc_err> parse_exp(){

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
                    next_expr = binary_op::add(
                        std::move(*next_expr), 
                        std::move(*next_expr_2)
                    );    
                }
                else{
                    next_expr = binary_op::sub(
                        std::move(*next_expr), 
                        std::move(*next_expr_2)
                    );  
                }
            }

            return next_expr;
        }

        constexpr std::expected<expr_ptr_t, calc_err> parse_mul_div(){

            auto next_expr = parse_exponent();
            if(!next_expr){
                return next_expr;
            }
            
            while(t.match(tokenizer::TOKEN_TYPE::ASTERISK) || 
                t.match(tokenizer::TOKEN_TYPE::SLASH)
            ){
                auto next = t.next();

                auto next_expr_2 = parse_exponent();
                if(!next_expr_2){
                    return next_expr_2;
                }

                if(next->type == tokenizer::TOKEN_TYPE::ASTERISK){
                    next_expr = binary_op::mult(
                        std::move(*next_expr), 
                        std::move(*next_expr_2)
                    ); 
                }
                else{
                    next_expr = binary_op::div(
                        std::move(*next_expr), 
                        std::move(*next_expr_2)
                    ); 
                }
            }

            return next_expr;
        }

        constexpr std::expected<expr_ptr_t, calc_err> parse_exponent(){
            auto next_expr = parse_sign();
            if(!next_expr){
                return next_expr;
            }
            
            if(t.match(tokenizer::TOKEN_TYPE::EXPONENT)){
                t.next();

                auto next_expr_2 = parse_sign();
                if(!next_expr_2){
                    return next_expr_2;
                }

                return binary_op::exponent(std::move(*next_expr), std::move(*next_expr_2));
               
            }

            return next_expr;
        }

        constexpr std::expected<expr_ptr_t, calc_err> parse_sign(){

            bool is_pos = true;

            if(t.match(tokenizer::TOKEN_TYPE::MINUS)){
                t.next();
                is_pos = false;
            }
            
            auto next_expr = parse_factorial();
            if(!next_expr || is_pos){
                return next_expr;
            }
            
            return unary_op::neg(std::move(*next_expr));
        }
        
        constexpr std::expected<expr_ptr_t, calc_err> parse_factorial(){
            
            auto next_expr = parse_atom();
            if(!next_expr){
                return next_expr;
            }

            if(t.match(tokenizer::TOKEN_TYPE::FACTORIAL)){
                t.next();
                return unary_op::factorial(std::move(*next_expr));
            }
            
            return next_expr;
        }

        constexpr std::expected<expr_ptr_t, calc_err> parse_atom(){
            using enum calc_err_type_t;

            std::expected<expr_ptr_t, calc_err> tmp;
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

            // string to num_t
            constexpr auto ston = 
                [](auto start, const auto end) -> std::optional<num_t> {

                    num_t ret = 0;
                    while(start != end){

                        if(!(*start >= '0' && *start <= '9')){
                            return std::nullopt;
                        }

                        ret = ret * 10 + static_cast<num_t>(*start - '0');

                        ++start;
                    }

                    return ret;
                };

            if(n.size() == 0 || n.size() > 20){
                return std::nullopt;
            }

            auto decimal_point = std::find(std::begin(n), std::end(n), '.');

            auto numerator_opt = ston(std::begin(n), decimal_point);
            if(!numerator_opt || decimal_point == std::end(n)){
                return numerator_opt;
            }

            ++decimal_point; // go to the first decimal digit            
            num_t div = static_cast<num_t>(
                std::distance(decimal_point, std::end(n))
            );
            
            auto denom_opt = ston(decimal_point, std::end(n));
            if(!denom_opt){
                return denom_opt;
            }

            return *numerator_opt + *denom_opt / std::pow(10., div);
        }

    };
}

}

#endif