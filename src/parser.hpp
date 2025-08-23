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
    template<typename evaluation_t>
    requires std::is_arithmetic<evaluation_t>::value
    class parser{

        struct expr{
            constexpr virtual evaluation_t evaluate() const = 0;
            constexpr virtual ~expr() = default;
        };

        struct add : expr{
            std::unique_ptr<expr> l;
            std::unique_ptr<expr> r;
            add() = delete;

            constexpr add(std::unique_ptr<expr>&& l2, std::unique_ptr<expr>&& r2) noexcept: 
                l(std::move(l2)), r(std::move(r2))
            {}

            constexpr evaluation_t evaluate() const {
                return l->evaluate() + r->evaluate();
            }

            constexpr ~add() = default;
        };

        struct atom : expr{
            std::unique_ptr<expr> data;
            constexpr evaluation_t evaluate() const = 0;
            atom() = delete;
            atom(const atom& other) = delete;
            atom(atom&& other) = delete;
            constexpr atom(std::unique_ptr<expr>&& ptr) noexcept: data(std::move(ptr)){}

            atom operator=(const atom& other) = delete;
            atom operator=(atom&& other) = delete;
        };

        struct unary_minus : atom{
            explicit constexpr unary_minus(std::unique_ptr<expr>&& next) noexcept: atom::atom(std::move(next)){}
            constexpr evaluation_t evaluate() const {
                return -atom::data->evaluate();
            };
        };

        struct lit : atom{
            evaluation_t value;
            explicit constexpr lit(evaluation_t v) noexcept: atom::atom(nullptr), value(v){}

            constexpr evaluation_t evaluate() const {
                return value;
            }
        };

        std::unique_ptr<expr> root;
        tokenizer t;

    public:
        constexpr parser() = default;

        explicit constexpr parser(std::string_view input): root(nullptr){
            parse(input);
        }

        constexpr void parse(std::string_view input){
            root.reset();
            if(!t.tokenize(input))
                return;

            auto tmp = parse();

            if(!tmp){
                println("{}", tmp.error());
                return;
            }

            if(t.has_more_tokens()){
            std::puts("Invalid expression, too many tokens ........ evaluation_tODO");
                return;
            }

            root = std::move(*tmp);
        }

        constexpr std::expected<std::unique_ptr<expr>, std::string> parse(){

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

        constexpr std::expected<std::unique_ptr<expr>, std::string> parse_atom(){
            std::expected<std::unique_ptr<expr>, std::string> tmp;
            std::optional<evaluation_t> lit_val;

            auto tok = t.next();

            if(!tok.has_value()){
                return std::unexpected("Missing token here, expected ...... evaluation_tODO");
            }
            
            switch (tok->type){
            case tokenizer::TOKEN_TYPE::OPEN_PAR:
                tmp = parse();
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
        constexpr std::optional<evaluation_t> evaluate() const{
            if(root == nullptr)
                return std::nullopt;

            return root->evaluate();
        }

        constexpr std::optional<evaluation_t> lit_convert(std::string_view n){
            evaluation_t ret = 0;
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

    template<typename evaluation_t>
    constexpr std::optional<evaluation_t> evaluate(std::string_view str)
    requires std::is_arithmetic<evaluation_t>::value{
        return parser<evaluation_t>(str).evaluate();
    }
}

// still good idea if exprssion will ever use complex numbers
// template<typename evaluation_t>
// concept Evaluatable = requires(evaluation_t node){
//     {node.evaluate()} -> std::convertible_to<evaluation_t>; // change to a more appropriate type
// };


#endif