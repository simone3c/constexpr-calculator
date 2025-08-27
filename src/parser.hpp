#ifndef _Mevaluation_t_PARSER_
#define _MY_PARSER_

#include <memory>
#include <cassert>
#include <expected>
#include <type_traits>
#include <functional>

#include "tokenizer.hpp"



/*
    EXPR: MUL ('+' MUL)? // DIV...
    MUL: ATOM ('*' ATOM)? // SUB...
    ATOM: '-'? POS_ATOM
    POS_ATOM: LIT | '(' EXPR ')' // '^', SIN, LOG, ...
    LIT: int // double


*/

namespace ev{

    namespace {
        
        template<typename eval_t>
        requires std::is_arithmetic<eval_t>::value
        class parser{
            
        struct expr{
            constexpr virtual eval_t evaluate() const = 0;
            constexpr virtual ~expr() = default;
        };
            
        using expr_ptr_t = std::unique_ptr<expr>;

        struct add : expr{
            expr_ptr_t l;
            expr_ptr_t r;

            constexpr add(expr_ptr_t&& l2, expr_ptr_t&& r2) noexcept: 
                l(std::move(l2)), r(std::move(r2))
            {}

            constexpr eval_t evaluate() const {
                return l->evaluate() + r->evaluate();
            }

            constexpr ~add() = default;
        };

        struct atom : expr{
            expr_ptr_t data;

            constexpr atom(expr_ptr_t&& ptr) noexcept: data(std::move(ptr)){}
        };

        struct unary_minus : atom{
            explicit constexpr unary_minus(expr_ptr_t&& next) noexcept: atom::atom(std::move(next)){}
            constexpr eval_t evaluate() const {
                return -atom::data->evaluate();
            };
        };

        struct lit : atom{
            eval_t value;
            explicit constexpr lit(eval_t v) noexcept: atom::atom(nullptr), value(v){}

            constexpr eval_t evaluate() const {
                return value;
            }
        };

        expr_ptr_t root;
        tokenizer t;
        std::string expr;

    public:
        constexpr parser() = default;

        constexpr ev::error parse(std::string_view input){
            using enum ev::error::err_type_t;
            root.reset();

            ev::error err = t.tokenize(input);
            if(err.err_type != NO_ERROR)
                return err;

            auto tmp = parse_exp();

            if(!tmp){
                return ev::error::basic_error(UNEXPECTED_TOKEN, tmp.error());
            }

            if(t.has_more_tokens()){
                return ev::error::basic_error(UNEXPECTED_TOKEN, "Unexpected expression terminator found");
            }

            root = std::move(*tmp);

            return ev::error::no_error();
        }

        constexpr std::expected<expr_ptr_t, std::string> parse_exp(){

            auto next_expr = parse_atom();
            if(!next_expr){
                return next_expr;
            }
            
            // SUB ....
            while(t.match(tokenizer::TOKEN_TYPE::PLUS)){
                t.next();

                auto next_expr_2 = parse_atom();
                if(!next_expr_2){
                    return next_expr_2;
                }

                next_expr = std::make_unique<add>(std::move(*next_expr), std::move(*next_expr_2));
            }

            return next_expr;
        }

        constexpr std::expected<expr_ptr_t, std::string> parse_atom(){
            std::expected<expr_ptr_t, std::string> tmp;
            std::optional<eval_t> lit_val;

            auto tok = t.next();

            if(!tok.has_value()){
                return std::unexpected("Missing token here, expected ...... evaluation_tODO");
            }
            
            switch (tok->type){
            case tokenizer::TOKEN_TYPE::OPEN_PAR:
                tmp = parse_exp();
                if(!tmp){
                    return std::unexpected("Empty paranthesis");
                }
                if(!t.consume(tokenizer::TOKEN_TYPE::CLOSED_PAR)){
                    return std::unexpected("Missing a closed paranthesis ')'");
                }
                break;

            case tokenizer::TOKEN_TYPE::MINUS:
                if(t.match(tokenizer::TOKEN_TYPE::MINUS)){
                    return std::unexpected("Cannot have two '-' symbol back-to-back");
                }

                tmp = parse_atom();
                if(!tmp){
                    return std::unexpected("Unary minus without argument");
                }
                tmp = std::make_unique<unary_minus>(std::move(*tmp));
                break;

            case tokenizer::TOKEN_TYPE::LIT:
                lit_val = lit_convert(tok->val);
                if(!lit_val){
                    return std::unexpected(std::format("Invalid literal {}", tok->val));
                }
                tmp = std::make_unique<lit>(*lit_val); 
                break;

            // sin, cos, ...

            default:
                return std::unexpected("Bad expression, expected ..... evaluation_tODO");
            }

            return tmp;
        }

        // when dvision will be a thing, maybe the evaluation will need to return
        // an expected in order to handle divison by 0... sqrt(-1)....
        constexpr std::expected<eval_t, ev::error> evaluate(std::string_view str){
            using enum ev::error::err_type_t;
            ev::error err = parse(str);
            if(err.err_type != NO_ERROR){
                return std::unexpected(err);
            }

            if(root == nullptr)
                return std::unexpected(ev::error::basic_error(INVALID_EXPR, "Parsing failed"));

            return root->evaluate();
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
    constexpr std::expected<eval_t, ev::error> evaluate(std::string_view str)
    requires std::is_arithmetic<eval_t>::value{
        return parser<eval_t>().evaluate(str);
    }
}

// still good idea if exprssion will ever use complex numbers
// template<typename evaluation_t>
// concept Evaluatable = requires(evaluation_t node){
//     {node.evaluate()} -> std::convertible_to<evaluation_t>; // change to a more appropriate type
// };


#endif