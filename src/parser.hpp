#ifndef _MY_PARSER_
#define _MY_PARSER_

#include <memory>
#include <cassert>
#include <expected>
#include <type_traits>
#include <functional>

#include "tokenizer.hpp"

// still good idea if exprssion will ever use complex numbers
// template<typename evaluation_t>
// concept Evaluatable = requires(evaluation_t node){
//     {node.evaluate()} -> std::convertible_to<evaluation_t>; // change to a more appropriate type
// };


/*
    EXPR: MUL (('+' MUL_DIV)? | ('-' MUL_DIV)?)
    MUL_DIV: ATOM (('*' ATOM)? | ('/' ATOM)?)
    ATOM: '-'? POS_ATOM
    POS_ATOM: LIT | '(' EXPR ')' // '^', SIN, LOG, ...
    LIT: int // double
*/

namespace ev{

    template<typename eval_t>
    requires std::is_arithmetic<eval_t>::value
    using ret_val_t = std::expected<eval_t, calc_err>;

namespace {

    template<typename eval_t>
    requires std::is_arithmetic<eval_t>::value
    struct expr{
        constexpr virtual ret_val_t<eval_t> evaluate() const = 0;
        constexpr virtual ~expr() = default;
    };

    template<typename eval_t>
    requires std::is_arithmetic<eval_t>::value
    using expr_ptr_t = std::unique_ptr<expr<eval_t>>;

    template<typename eval_t>
    requires std::is_arithmetic<eval_t>::value
    struct mult : expr<eval_t>{
        expr_ptr_t<eval_t> l;
        expr_ptr_t<eval_t> r;

        constexpr mult(expr_ptr_t<eval_t>&& l2, expr_ptr_t<eval_t>&& r2) noexcept: 
            l(std::move(l2)), 
            r(std::move(r2))
        {}

        constexpr ret_val_t<eval_t> evaluate() const {
            auto left = l->evaluate();
            if(!l){
                return l;
            }

            auto right = r->evaluate();
            if(!right){
                return right;
            }

            return *left * *right;
        }

        constexpr ~mult() = default;
    };

    template<typename eval_t>
    requires std::is_arithmetic<eval_t>::value
    struct div : expr<eval_t>{
        expr_ptr_t<eval_t> n;
        expr_ptr_t<eval_t> d;

        constexpr div(expr_ptr_t<eval_t>&& l2, expr_ptr_t<eval_t>&& r2) noexcept: 
            n(std::move(l2)), 
            d(std::move(r2))
        {}

        constexpr ret_val_t<eval_t> evaluate() const {
            auto denom = d->evaluate();
            if(!denom){
                return denom;
            }
            if(*denom == 0.){
                return std::unexpected(
                    ev::calc_err::error_message(
                        ev::calc_err_type_t::DIVSION_BY_ZERO, 
                        "Division by 0 detected"
                    )
                );
            }
            return *n->evaluate() / *denom;
        }

        constexpr ~div() = default;
    };

    template<typename eval_t>
    requires std::is_arithmetic<eval_t>::value
    struct add : expr<eval_t>{
        expr_ptr_t<eval_t> l;
        expr_ptr_t<eval_t> r;

        constexpr add(expr_ptr_t<eval_t>&& l2, expr_ptr_t<eval_t>&& r2) noexcept: 
            l(std::move(l2)), 
            r(std::move(r2))
        {}

        constexpr ret_val_t<eval_t> evaluate() const {
            auto left = l->evaluate();
            if(!left){
                return left;
            }

            auto right = r->evaluate();
            if(!right){
                return right;
            }

            return *left + *right;
        }

        constexpr ~add() = default;
    };

    template<typename eval_t>
    requires std::is_arithmetic<eval_t>::value
    struct sub : expr<eval_t>{
        expr_ptr_t<eval_t> l;
        expr_ptr_t<eval_t> r;

        constexpr sub(expr_ptr_t<eval_t>&& l2, expr_ptr_t<eval_t>&& r2) noexcept: 
            l(std::move(l2)), 
            r(std::move(r2))
        {}

        constexpr ret_val_t<eval_t> evaluate() const {
            auto left = l->evaluate();
            if(!left){
                return left;
            }

            auto right = r->evaluate();
            if(!right){
                return right;
            }

            return *left - *right;
        }

        constexpr ~sub() = default;
    };

    template<typename eval_t>
    requires std::is_arithmetic<eval_t>::value
    struct atom : expr<eval_t>{
        expr_ptr_t<eval_t> data;

        explicit constexpr atom(expr_ptr_t<eval_t>&& ptr) noexcept: 
            data(std::move(ptr))
        {}
    };

    template<typename eval_t>
    requires std::is_arithmetic<eval_t>::value
    struct unary_minus : atom<eval_t>{
        explicit constexpr unary_minus(expr_ptr_t<eval_t>&& next) noexcept: 
            atom<eval_t>::atom(std::move(next))
        {}

        constexpr ret_val_t<eval_t> evaluate() const {
            auto val = atom<eval_t>::data->evaluate();
            if(!val){
                return val;
            }
            return -*val;
        };
    };

    template<typename eval_t>
    requires std::is_arithmetic<eval_t>::value
    struct lit : expr<eval_t>{
        eval_t value;
        explicit constexpr lit(eval_t v) noexcept: 
            value(v)
        {}

        constexpr ret_val_t<eval_t> evaluate() const {
            return value;
        }
    };
    
    template<typename eval_t>
    requires std::is_arithmetic<eval_t>::value
    class parser{
        
        expr_ptr_t<eval_t> root;
        tokenizer t;
        std::string expr;

    public:
        constexpr parser() = default;

        constexpr std::expected<eval_t, calc_err> evaluate(std::string_view str){
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

        constexpr std::expected<expr_ptr_t<eval_t>, calc_err> parse_exp(){

            auto next_expr = parse_atom();
            if(!next_expr){
                return next_expr;
            }
            
            while(t.match(tokenizer::TOKEN_TYPE::PLUS) || t.match(tokenizer::TOKEN_TYPE::MINUS)){
                auto next = t.next();

                auto next_expr_2 = parse_atom();
                if(!next_expr_2){
                    return next_expr_2;
                }

                if(next->type == tokenizer::TOKEN_TYPE::PLUS){
                    next_expr = std::make_unique<add<eval_t>>(
                        std::move(*next_expr), 
                        std::move(*next_expr_2)
                    );
                }
                else{
                    next_expr = std::make_unique<sub<eval_t>>(
                        std::move(*next_expr), 
                        std::move(*next_expr_2)
                    );
                }
            }

            return next_expr;
        }

        constexpr std::expected<expr_ptr_t<eval_t>, calc_err> parse_atom(){
            using enum calc_err_type_t;

            std::expected<expr_ptr_t<eval_t>, calc_err> tmp;
            std::optional<eval_t> lit_val;

            auto tok = t.next();

            if(!tok.has_value()){
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

            case tokenizer::TOKEN_TYPE::MINUS:
                if(t.match(tokenizer::TOKEN_TYPE::MINUS)){
                    return std::unexpected(
                        calc_err::error_with_wrong_token(
                            UNEXPECTED_TOKEN, 
                            "Unexpected '-' after another '-' symbol", 
                            expr, 
                            tok->start, 
                            tok->end
                        )
                    );
                }

                tmp = parse_atom();
                if(!tmp){
                    return tmp;
                }
                tmp = std::make_unique<unary_minus<eval_t>>(std::move(*tmp));
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
                tmp = std::make_unique<lit<eval_t>>(*lit_val); 
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

        constexpr std::optional<eval_t> lit_convert(std::string_view n){
            eval_t ret = 0;
            size_t i = 0;
            bool neg = false;

            if(n.size() == 0){
                return std::nullopt;
            }
            
            if(n[0] == '-'){
                if(n.size() == 1){
                    return std::nullopt;
                }
                neg = true;
                i = 1;
            }
            
            while(i < n.size()){
                if(!(n[i] >= '0' && n[i] <= '9'))
                    return std::nullopt;

                ret *= 10;
                ret += (n[i] - '0');
                ++i;
            }

            return neg ? -ret : ret;
        }

    };
}

    template<typename eval_t>
    constexpr std::expected<eval_t, calc_err> evaluate(std::string_view str)
    requires std::is_arithmetic<eval_t>::value{
        return parser<eval_t>().evaluate(str);
    }
}

#endif