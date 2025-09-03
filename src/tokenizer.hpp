#ifndef _MY_TOKENIZER_
#define _MY_TOKENIZER_

#include <string>
#include <string_view>
#include <list>
#include <print>
#include <algorithm>
#include <cctype>
#include <optional>
#include <cstring>
#include <utility>

#include "ctre/single-header/ctre.hpp"
#include "error.hpp"

using std::println;
using std::print;

#define RED   "\x1B[1;31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

#define regex "(?<lit>([0-9]*[.])?[0-9]+)|"\
              "(?<open_par>\\()|"\
              "(?<closed_par>\\))|"\
              "(?<plus>\\+)|"\
              "(?<minus>-)|"\
              "(?<exponent>\\^)|"\
              "(?<asterisk>\\*)|"\
              "(?<slash>\\/)|"\
              "(?<factorial>!)|"\
              "(?<abs>abs)|"\
              "(?<floor>floor)|"\
              "(?<ceil>ceil)|"\
              "(?<space>\\s+)|.+"

namespace calc{
namespace{

    struct tokenizer{
        using enum calc::calc_err_type_t;

        enum class TOKEN_TYPE{
            LIT,
            OPEN_PAR,
            CLOSED_PAR,
            PLUS,
            MINUS,
            ASTERISK,
            SLASH,
            FACTORIAL,
            EXPONENT,
            ABS,
            FLOOR,
            CEIL,

        };

        struct token{
            TOKEN_TYPE type;
            // complete expression and ptrs to its portion which contains
            // the actual token string
            std::string full_expr;
            size_t start;
            size_t end;
        };

        //std::list<token> tokens since c++26 when std::list will be constexpr
        std::vector<token> tokens;
        std::string str;

        constexpr tokenizer() = default;

        [[nodiscard]] constexpr std::optional<calc_err> tokenize(std::string_view input){

            tokens.clear();
            str.clear();
            size_t start, end = 0;

            if(input.size() == 0){
                return calc_err::error_message(
                    EMPTY_EXPRESSION, 
                    "Insert at least one character"
                );
            }

            str = input;

            auto res = ctre::tokenize<regex>(str);
            for(const auto& r : res){
                TOKEN_TYPE t;

                start = end;
                end += r.str().size();

                //println("Token '{}'", r.str());


                if(r.get<"lit">()){
                    t = TOKEN_TYPE::LIT;
                }
                else if(r.get<"open_par">()){
                    t = TOKEN_TYPE::OPEN_PAR;
                }
                else if(r.get<"closed_par">()){
                    t = TOKEN_TYPE::CLOSED_PAR;
                }
                else if(r.get<"plus">()){
                    t = TOKEN_TYPE::PLUS;
                }
                else if(r.get<"minus">()){
                    t = TOKEN_TYPE::MINUS;
                }
                else if(r.get<"asterisk">()){
                    t = TOKEN_TYPE::ASTERISK;
                }
                else if(r.get<"slash">()){
                    t = TOKEN_TYPE::SLASH;
                }
                else if(r.get<"factorial">()){
                    t = TOKEN_TYPE::FACTORIAL;
                }
                else if(r.get<"exponent">()){
                    t = TOKEN_TYPE::EXPONENT;
                }
                else if(r.get<"abs">()){
                    t = TOKEN_TYPE::ABS;
                }
                else if(r.get<"floor">()){
                    t = TOKEN_TYPE::FLOOR;
                }
                else if(r.get<"ceil">()){
                    t = TOKEN_TYPE::CEIL;
                }
                else if(r.get<"space">()){
                    continue;
                }
                else{
                    auto err = calc_err::error_with_wrong_token(
                        UNKNOWN_TOKEN, 
                        "Unknown symbol found", 
                        str, 
                        start,
                        end
                    );
                    // clear internal status
                    tokens.clear();
                    str.clear();

                    return err;
                }
                tokens.emplace_back(t, str, start, end);
            }

            return std::nullopt;
        }

        [[nodiscard]] constexpr std::optional<token> peek() const {
            if(tokens.empty())
                return std::nullopt;

            return tokens.front();
        }

        [[maybe_unused]] constexpr std::optional<token> next(){
            auto ret = peek();
            if(ret){
                //std::println("{}", ret->val);
                tokens.erase(tokens.begin());
                // tokens.pop_front() since c++26 when std::list will be constexpr
            }

            return ret;
        }

        [[nodiscard]] constexpr bool match(TOKEN_TYPE other) const {
            return peek().transform([&other]
                (const token& tok){
                    return tok.type == other;      
                }
            )
            .value_or(false);
        }

        [[nodiscard]] constexpr bool consume(TOKEN_TYPE other) {
            auto ret = match(other);
            next();
            return ret;
        }

        [[nodiscard]] constexpr bool has_more_tokens() const {
            return tokens.size() > 0;
        }
    };
};
};

#endif